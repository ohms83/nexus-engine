//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/RenderCommand.h"

#include "entt/entity/registry.hpp"

NXS_NAMESPACE
{
    class RenderSystem;
    class Scene;
    class Shader;
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class ISceneRenderer
    {
    public:
        virtual ~ISceneRenderer() = default;
        virtual void Render(RenderSystem& renderSystem, const entt::registry& registry) = 0;
    };

    class BasicSceneRenderer final :public ISceneRenderer
    {
    public:
        BasicSceneRenderer(const RenderSystem& renderSystem);
        void Render(RenderSystem& renderSystem, const entt::registry& registry) override;
        void RenderDepthPrePass(RenderSystem& renderSystem, const entt::registry& registry);
    private:
        Ref<Shader> m_depthShader;
        std::vector<RenderCommand> m_commandBuffer;
    };
}
