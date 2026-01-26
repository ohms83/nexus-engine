//
// Created by nutta on 8/18/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/resource/ResourceManager.h"
#include "nexus/graphics/RenderingInterface.h"
#include "Material.h"
#include "Model.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "ShaderManager.h"

NXS_NAMESPACE
{
    class ModelManager final : public ResourceManager
    {
    public:
        ModelManager(
            const Ref<RenderingInterface>& renderingInterface,
            const Ref<TextureManager>& textureManager,
            const Ref<MaterialManager>& materialManager,
            const Ref<ShaderManager>& shaderManager);
    };
}
