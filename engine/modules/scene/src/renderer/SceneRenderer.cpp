//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <cmath>
#include <limits>

#include "nexus/debug/Logger.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/Shader.h"
#include "nexus/graphics/RenderCommandBatcher.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/geom/Frustum.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/memory/OwningBuffer.h"
#include "nexus/scene/RenderGraph.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/math/Matrix.h"

#include "Remotery.h"
#include "nexus/graphics/GraphicsProfiler.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

bool SceneRenderer::Init(const IGpuResourceFactory& factory)
{
    if (m_initialized) return true;

    for (auto& [queue, pass] : m_passesByQueue)
    {
        if (!pass->Init(factory)) return false;
    }

    for (auto& [queue, pass] : m_perFramePasses)
    {
        if (!pass->Init(factory)) return false;
    }
    return m_initialized = true;
}

void SceneRenderer::Render(RenderSystem &rs, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    SceneRenderingContext sceneCtx = {
        .scene = &scene
    };

    for (auto& [queue, pass] : m_perFramePasses)
    {
        pass->Begin(rs, sceneCtx);
        pass->Render(rs, sceneCtx);
        pass->End(rs, sceneCtx);
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
    }
}

void SceneRenderer::RenderToScreen(RenderSystem& rs)
{
    
}