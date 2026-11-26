#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class ShadowPass : public RenderPass
    {
    public:
        ShadowPass();
        virtual ~ShadowPass() = default;

        bool Init(const IGpuResourceFactory& factory) override;
        void Render(RenderSystem& rs, const SceneRenderingContext& sceneCtx) override;

    private:
        void Begin_Internal(RenderSystem& rs, SceneRenderingContext& sceneCtx) override;
        void End_Internal(RenderSystem& rs, SceneRenderingContext& sceneCtx) override;

        glm::mat4 m_lightSpaceMatrix{1.0f};
        Identifier m_shadowLightId = NXS_INVALID_ID;
        bool m_shadowMapRendered = false;
    };
}