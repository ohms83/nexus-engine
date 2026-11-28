//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <array>
#include <cmath>

#include "nexus/debug/Logger.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/Shader.h"
#include "nexus/graphics/GpuProgram.h"
#include "nexus/graphics/VertexBuffer.h"
#include "nexus/graphics/IndexBuffer.h"
#include "nexus/memory/BorrowBuffer.h"
#include "nexus/scene/Scene.h"
#include "nexus/task/RepeatTask.h"
#include "nexus/task/IntervalTask.h"
#include "nexus/time/Timer.h"
#include "nexus/math/Math.h"

#include "Remotery.h"
#include "nexus/graphics/GraphicsProfiler.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

SceneRenderer::SceneRenderer()
{
    m_timer = NewRef<Timer>(NewRef<StandardTimeSource>());
    m_timer->Start();
}

SceneRenderer::~SceneRenderer()
{
    m_timer.reset();
    m_viewportResizeConnection.disconnect();
}

bool SceneRenderer::Init(const IGpuResourceFactory* factory, RenderSystem& rs)
{
    if (m_initialized) return true;

    Shader screenShader("_Screen", 0);
    constexpr auto vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPosition;
        out vec2 vScreenUv;
        void main()
        {
            vScreenUv = aPosition * 0.5 + 0.5;
            gl_Position = vec4(aPosition, 0.0, 1.0);
        }
    )";
    constexpr auto fragmentSource = R"(
        #version 330 core
        in vec2 vScreenUv;
        uniform sampler2D _ScreenTexture;
        uniform float _Exposure;
        out vec4 FragColor;
        void main()
        {
            vec3 hdrColor = texture(_ScreenTexture, vScreenUv).rgb;
            vec3 exposed = hdrColor * _Exposure;
            // reinhard tone mapping
            vec3 mapped = exposed / (exposed + vec3(1.0));
            // gamma correction 
            const float gamma = 2.2;
            mapped = pow(mapped, vec3(1.0 / gamma));
            FragColor = vec4(mapped, 1.0);
        }
    )";
    if (!screenShader.CompileFromSource(
        *factory,
        vertexSource,
        fragmentSource,
        ""))
    {
        return false;
    }
    m_screenShader = screenShader.GetGpuProgram();

    static std::array<float, 6> fullscreenVertices = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f,
    };
    static std::array<uint32_t, 3> fullscreenIndices = { 0, 1, 2 };

    m_screenVertexBuffer = factory->CreateVertexBuffer();
    m_screenVertexBuffer->Begin()
        .SetVertices(std::make_shared<BorrowBuffer>(fullscreenVertices))
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute::VertexPosition2D)
        .Build();

    m_screenIndexBuffer = factory->CreateIndexBuffer();
    m_screenIndexBuffer->Begin()
        .SetIndices(std::make_shared<BorrowBuffer>(fullscreenIndices),
            FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
        .Build();

    for (auto& [queue, pass] : m_passesByQueue)
    {
        if (!pass->Init(*factory)) return false;
    }

    for (auto& [queue, pass] : m_perFramePasses)
    {
        if (!pass->Init(*factory)) return false;
    }

    m_timer->ScheduleAction([&]() {
        m_sampleLuminance = true;
    }, 1);
    m_sampleLuminance = false;

    m_viewportResizeConnection = rs.onViewportResize.connect(&SceneRenderer::OnViewportResize, this);
    m_gpuResourceFactory = factory;
    m_initialized = true;
    const auto& config = rs.GetGraphicsConfig();
    CreateOffscreenBuffer(config.screenWidth, config.screenHeight);
    return true;
}

void SceneRenderer::SetTaskScheduler(Ref<TaskScheduler> scheduler)
{
    if (m_taskScheduler)
    {
        if (m_luminanceSamplingTask != NXS_INVALID_ID) m_taskScheduler->StopTask(m_luminanceSamplingTask);
        if (m_autoExposureEnabled != NXS_INVALID_ID) m_taskScheduler->StopTask(m_autoExposureEnabled);
    }
    m_taskScheduler = scheduler;

    auto task = NewRef<RepeatTask>(-1, [&]() {
        return UpdateAutoExposure();
    });
    m_luminanceSamplingTask = m_taskScheduler->ScheduleTask(task);
}

void SceneRenderer::Render(RenderSystem &rs, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    NXS_ASSERT_MSG(m_initialized, "Scene renderer hasn't been initialized!!");
    SceneRenderingContext sceneCtx = {
        .scene = &scene
    };

    for (auto& [queue, pass] : m_perFramePasses)
    {
        if (pass->targetType != RenderTargetType::Offscreen && m_screenBuffer) {
            m_screenBuffer->Bind(rs);
        }

        pass->Begin(rs, sceneCtx);
        pass->Render(rs, sceneCtx);
        pass->End(rs, sceneCtx);

        if (pass->targetType != RenderTargetType::Offscreen && m_screenBuffer) {
            m_screenBuffer->Unbind(rs);
        }
    }

    Array<RenderCommand> commands;
    commands.reserve(1024);

    struct QueuedRenderPass
    {
        int priority;
        RenderPass* pass;
    };
    struct ViewPasses
    {
        const ISceneView* view;
        ViewContext viewCtx;
        Array<QueuedRenderPass> passes;
    };
    Array<ViewPasses> views;
    for (const auto view : ISceneView::GetViewRegistry())
    {
        const auto* node = view->GetViewNode();
        if (node && (!node->IsActive() || node->GetRegistry() != scene.GetRegistry())) continue;

        auto viewCtx = view->CreateViewContext(scene);
        ViewPasses viewPasses {
            view,
            viewCtx,
        };

        const auto& renderProfile = viewCtx.profile;
        const auto addPass = [&](RenderPassQueue queue, bool selected) {
            if (!selected) return;
            const auto itr = m_passesByQueue.find(queue);
            if (itr != m_passesByQueue.end() && itr->second->enabled) {
                viewPasses.passes.emplace_back(QueuedRenderPass {
                    INT_CAST(itr->first),
                    itr->second.get()
                });
            }
        };
        addPass(RenderPassQueue::DepthFill, renderProfile.depthFill);
        addPass(RenderPassQueue::Opaque, !renderProfile.IsCull(CullingFlags::Opaque));
        addPass(RenderPassQueue::Environment, !renderProfile.IsCull(CullingFlags::Environment));
        addPass(RenderPassQueue::Alpha, !renderProfile.IsCull(CullingFlags::Alpha));
        addPass(RenderPassQueue::Overlay, !renderProfile.IsCull(CullingFlags::Overlay));
        std::ranges::sort(viewPasses.passes, {}, &QueuedRenderPass::priority);
        views.push_back(viewPasses);
    }

    for (const auto& viewPasses : views)
    {
        const auto& viewCtx = viewPasses.viewCtx;
        if (viewCtx.target) {
            viewCtx.target->Bind(rs);
        }
        else if (m_screenBuffer) {
            m_screenBuffer->Bind(rs);
        }

        viewCtx.ClearView(*rs.GetRenderInterface());

        for (const auto& queuedPass : viewPasses.passes)
        {
            auto& pass = queuedPass.pass;

            pass->Begin(rs, sceneCtx);
            pass->Render(rs, viewCtx, sceneCtx);
            pass->End(rs, sceneCtx);
        }

        if (viewCtx.target) {
            viewCtx.target->Unbind(rs);
        }
        else if (m_screenBuffer) {
            m_screenBuffer->Unbind(rs);
        }
    }

    RenderToScreen(rs);
}

void SceneRenderer::RenderToScreen(RenderSystem& rs)
{
    if (!m_screenBuffer || !m_screenShader ||
        !m_screenVertexBuffer || !m_screenIndexBuffer)
    {
        return;
    }

    const auto renderInterface = rs.GetRenderInterface();
    const bool framebufferSRGBSupported =
        renderInterface->IsFramebufferSRGBSupported();
    renderInterface->SetFramebufferSRGB(false);
    const auto& config = rs.GetGraphicsConfig();
    renderInterface->SetViewport(0, 0, config.screenWidth, config.screenHeight);

    PipelineState screenState;
    screenState.depthTest = false;
    screenState.depthWrite = false;
    screenState.stencilTest = false;
    screenState.overrideBlendMode = BlendMode::None;
    screenState.cullMode = PolygonFacing::None;
    screenState.frontFace = FrontFace::CounterClockWise;
    screenState.globalShader = m_screenShader;
    renderInterface->ApplyPipelineState(screenState);

    m_screenShader->Bind();
    m_screenShader->SetUniformTexture2D(
        "_ScreenTexture",
        m_screenBuffer->GetColorTextureProxy(),
        0);
    m_screenShader->SetUniformFloat("_Exposure", m_exposure);

    m_screenVertexBuffer->Bind();
    rs.DrawIndexed(m_screenIndexBuffer);
    m_screenVertexBuffer->Unbind();
    m_screenShader->Unbind();
    renderInterface->SetFramebufferSRGB(framebufferSRGBSupported);
}

void SceneRenderer::CreateOffscreenBuffer(int32_t width, int32_t height)
{
    if (!m_initialized || !m_gpuResourceFactory) return;

    auto desc = TextureDescription {
        .width = width,
        .height = height,
        .format = PixelFormat::RGBA_16F,
        .componentType = DataType::Float,
        .colorSpace = TextureColorSpace::Linear,
        .wrapModeS = TextureWrapMode::ClampToEdge,
        .wrapModeT = TextureWrapMode::ClampToEdge,
        .filterMin = TextureFilterMode::LinearMipmapLinear,
        .filterMag = TextureFilterMode::Linear,
        .numMips = Math::ComputeMaxMipLevel(width, height),
    };

    m_screenBuffer.reset();
    m_screenBuffer = m_gpuResourceFactory->CreateRenderTarget(desc);
    if (!m_screenBuffer || !m_screenBuffer->GetColorTextureProxy())
    {
        m_screenBuffer.reset();
        return;
    }

    m_screenTexels.reset();
    m_screenTexels = NewRef<CopyBuffer>(desc.GetBufferSize());
}

bool SceneRenderer::UpdateAutoExposure()
{
    rmt_ScopedCPUSample(SceneRenderer_UpdateAutoExposure, 0);
    if (!m_autoExposureEnabled || !m_screenBuffer) return false;

    m_timer->Tick();
    const float dt = std::max(0.0f, m_timer->GetDeltaTime());

    const auto hdrTexture = m_screenBuffer->GetColorTextureProxy();
    if (!hdrTexture)
    {
        LOG_ERROR(LogSceneRenderer, "The offscreen buffer doesn't have a color target!!");
        return false;
    }

    hdrTexture->Bind();

    if (m_sampleLuminance)
    {
        hdrTexture->GenerateMipmaps();
        m_sampleLuminance = false;
        m_timer->ScheduleAction([&]() {
            m_sampleLuminance = true;
        }, 1);
    }

    glm::vec4 color {0};
    const auto& desc = hdrTexture->GetDescription();
    hdrTexture->ReadTexel(desc.numMips - 1, &color, sizeof(glm::vec4));

    hdrTexture->Unbind();

    const float averageLuminance = 
            0.2126f * std::max(0.0f, color.r) +
            0.7152f * std::max(0.0f, color.g) +
            0.0722f * std::max(0.0f, color.b);

    if (!std::isfinite(averageLuminance) || averageLuminance <= 0.0f) return true;

    const float targetExposure = m_autoExposureTarget / averageLuminance;
    if (!Math::Approx(targetExposure, m_exposure))
    {
        const float clampedTargetExposure = std::clamp(targetExposure, 0.05f, 20.0f);
        const float blend = m_autoExposureSpeed <= 0.0f
            ? 1.0f
            : 1.0f - std::exp(-dt * m_autoExposureSpeed);

        m_exposure += (clampedTargetExposure - m_exposure) * blend;
    }
    return true;
}

void SceneRenderer::OnViewportResize(uint32_t w, uint32_t h)
{
    const uint32_t oldWidth = m_screenBuffer ? m_screenBuffer->GetWidth() : 0;
    const uint32_t oldHeight = m_screenBuffer ? m_screenBuffer->GetHeight() : 0;

    if (oldWidth == w && oldHeight == h) return;

    CreateOffscreenBuffer(INT_CAST(w), INT_CAST(h));
}