#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/resource/ResourceManager.h"

#include "Shader.h"
#include "ShaderLoader.h"

NXS_NAMESPACE
{
    class ShaderManager : public ResourceManager
    {
    public:
        explicit ShaderManager(Ref<RenderingInterface> renderingInterface)
        {
            RegisterLoader(std::make_unique<ShaderLoader>(renderingInterface));
        }
    };
}