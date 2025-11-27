//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderPass.h"

#include "entt/entity/registry.hpp"

#include <algorithm>

NXS_NAMESPACE
{
    class RenderSystem;
    class Scene;
    class Shader;
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class SceneRenderer
    {
    public:
        virtual ~SceneRenderer() = default;
        virtual void Render(RenderSystem& renderSystem, const entt::registry& registry) = 0;

        void RegisterRenderPass(const RenderPass& renderPass)
        {
            m_renderPasses.push_back(renderPass);
            std::ranges::sort(m_renderPasses, std::ranges::less{}, &RenderPass::priority);
        }

    protected:
        //! A list of render passes sorted by their priority.
        std::vector<RenderPass> m_renderPasses;
    };

    class ForwardSceneRenderer final :public SceneRenderer
    {
    public:
        ForwardSceneRenderer(const RenderSystem& renderSystem);
        void Render(RenderSystem& renderSystem, const entt::registry& registry) override;
        void RenderDepthPrePass(RenderSystem& renderSystem, const entt::registry& registry);
    };
}
