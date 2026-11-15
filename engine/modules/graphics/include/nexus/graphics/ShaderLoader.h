#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/resource/ResourceLoader.h"

#include "Shader.h"
#include "ShaderGenerator.h"

#include <string>

NXS_NAMESPACE
{
    class ShaderGenerator;

    class ShaderLoader : public IResourceLoader
    {
    public:
        Ref<Resource> Load(const std::string& path, uint32_t id) override;
        Ref<LoadResult> LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback) override;

    private:
        ShaderGenerator m_shaderGenerator;
    };
}