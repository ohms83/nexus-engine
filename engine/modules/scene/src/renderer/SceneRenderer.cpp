//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <array>

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

#include "Remotery.h"
#include "nexus/graphics/GraphicsProfiler.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

bool SceneRenderer::Init(const IGpuResourceFactory& factory, RenderSystem& rs)
{
    if (m_initialized) return true;

    const auto& config = rs.GetGraphicsConfig();
    CreateOffscreenBuffer(factory, config.screenWidth, config.screenHeight);

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
        out vec4 fragColor;
        void main()
        {
            fragColor = texture(_ScreenTexture, vScreenUv);
        }
    )";
    if (!screenShader.CompileFromSource(
        factory,
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

    m_screenVertexBuffer = factory.CreateVertexBuffer();
    m_screenVertexBuffer->Begin()
        .SetVertices(std::make_shared<BorrowBuffer>(fullscreenVertices))
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute::VertexPosition2D)
        .Build();

    m_screenIndexBuffer = factory.CreateIndexBuffer();
    m_screenIndexBuffer->Begin()
        .SetIndices(std::make_shared<BorrowBuffer>(fullscreenIndices),
            FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
        .Build();

    for (auto& [queue, pass] : m_passesByQueue)
    {
        if (!pass->Init(factory)) return false;
    }

    for (auto& [queue, pass] : m_perFramePasses)
    {
        if (!pass->Init(factory)) return false;
    }
    viewportResizeConnection = rs.onViewportResize.connect(&SceneRenderer::OnViewportResize, this);
    return m_initialized = true;
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
        if (pass->targetType != RenderTargetType::Offscreen) {
            m_screenBuffer->Bind(rs);
        }

        pass->Begin(rs, sceneCtx);
        pass->Render(rs, sceneCtx);
        pass->End(rs, sceneCtx);

        if (pass->targetType != RenderTargetType::Offscreen) {
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
        else {
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
        else {
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

    m_screenBuffer->Unbind(rs);

    const auto renderInterface = rs.GetRenderInterface();
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

    m_screenVertexBuffer->Bind();
    rs.DrawIndexed(m_screenIndexBuffer);
    m_screenVertexBuffer->Unbind();
    m_screenShader->Unbind();
}

void SceneRenderer::CreateOffscreenBuffer(const IGpuResourceFactory& factory, int32_t width, int32_t height)
{
    m_screenBuffer.reset();
    m_screenBuffer = GetGpuResourceFactory()->CreateRenderTarget( TextureDescription {
        .width = width,
        .height = height,
        .format = PixelFormat::RGBA_16F,
        .componentType = DataType::Float,
        .colorSpace = TextureColorSpace::SRgb,
        .wrapModeS = TextureWrapMode::ClampToEdge,
        .wrapModeT = TextureWrapMode::ClampToEdge,
    });
}

void SceneRenderer::OnViewportResize(uint32_t w, uint32_t h)
{
    const uint32_t oldWidth = m_screenBuffer ? m_screenBuffer->GetWidth() : 0;
    const uint32_t oldHeight = m_screenBuffer ? m_screenBuffer->GetHeight() : 0;

    if (oldWidth == w && oldHeight == h) return;

    CreateOffscreenBuffer(*GetGpuResourceFactory(), INT_CAST(w), INT_CAST(h));
}