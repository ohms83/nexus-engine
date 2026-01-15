#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/ResourceManager.h"

#include "Shader.h"
#include "ShaderLoader.h"

NXS_NAMESPACE
{
    class ShaderManager : public ResourceManager<Shader>
    {
    public:
        explicit ShaderManager(Ref<RenderingInterface> renderingInterface)
        {
            RegisterLoader(std::make_unique<ShaderLoader>(renderingInterface));
        }
    };
}