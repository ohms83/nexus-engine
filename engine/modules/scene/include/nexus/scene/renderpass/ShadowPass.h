#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class ShadowPass : public RenderPass
    {
    public:
        ShadowPass() = default;
        virtual ~ShadowPass() = default;

        bool Init() override;

    private:
        void Begin_Internal(RenderSystem& rs) const override;

        Ref<RenderTarget> m_shadowMap;
        Ref<GpuProgram> m_shadowProgram;
        glm::mat4 m_lightSpaceMatrix{1.0f};
        bool m_shadowMapReady = false;
    };
}