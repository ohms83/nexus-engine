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
        void Render(RenderSystem& rs, const ViewContext& viewCtx, SceneRenderingContext& sceneCtx) override;

    private:
        Ref<RenderTarget> m_shadowMap;
        Ref<GpuProgram> m_shadowProgram;
        glm::mat4 m_lightSpaceMatrix{1.0f};
    };
}