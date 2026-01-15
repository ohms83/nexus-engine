#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/ResourceLoader.h"

#include "Shader.h"
#include "ShaderGenerator.h"
#include "RenderingInterface.h"

#include <string>
#include <functional>

NXS_NAMESPACE
{
    class ShaderGenerator;

    class ShaderLoader : public IResourceLoader
    {
    public:
        explicit ShaderLoader(Ref<RenderingInterface> renderingInterface)
            : m_renderingInterface(renderingInterface)
        {
        }

        Ref<Resource> Load(const std::string& path, uint32 id) override;
        Ref<LoadResult> LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback) override;

    private:
        Ref<RenderingInterface> m_renderingInterface;
        ShaderGenerator m_shaderGenerator;
    };
}