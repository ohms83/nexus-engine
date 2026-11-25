#pragma once

#include "RenderPass.h"

NXS_NAMESPACE
{
    /**
     * @brief A render pass specifically for drawing the scene environment (eg., skybox).
     * This pass sits between Opaque and Alpha to minimize pixel overdraw.
     * The pass will only perform depth test but not depth write, assuming that
     * everything it draws is beyond the draw distance.
     */
    class EnvironmentPass final : public RenderPass
    {
        NXS_INHERIT(RenderPass);
    public:
        EnvironmentPass();
        bool Init(const IGpuResourceFactory& factory) override;

        void Render(RenderSystem& rs, const ViewContext& viewCtx, SceneRenderingContext& sceneCtx) override;

    private:
        // Skybox
        Ref<Mesh> m_skybox;
        Ref<GpuProgram> m_skyboxShader;
    };
}