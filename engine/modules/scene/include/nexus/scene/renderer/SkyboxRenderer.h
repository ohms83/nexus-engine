#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class IGpuResourceFactory;
    class GpuProgram;
    class Mesh;
    class RenderSystem;
    class Scene;
    class ISceneView;

    class SkyboxRenderer final
    {
    public:
        explicit SkyboxRenderer(const IGpuResourceFactory& gpuFactory);
        void Render(RenderSystem &rs, const Scene &scene, const ISceneView& viewPoint);
    private:
        Ref<Mesh> m_skybox;
        Ref<GpuProgram> m_shader;
    };
}