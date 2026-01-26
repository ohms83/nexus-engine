

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/resource/ResourceManager.h"
#include "TextureLoader.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    class TextureManager final : public ResourceManager
    {
    public:
        explicit TextureManager(Ref<RenderingInterface> renderingInterface)
        {
            RegisterLoader(std::make_unique<TextureLoader>(renderingInterface));
        }
    };
}