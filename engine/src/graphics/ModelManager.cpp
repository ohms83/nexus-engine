//
// Created by nutta on 8/18/2025.
//
#include "nexus/graphics/ModelManager.h"
#include "nexus/graphics/ModelLoader.h"

USING_NAMESPACE_NXS;

ModelManager::ModelManager(
    const Ref<RenderingInterface>& renderingInterface,
    const Ref<TextureManager>& textureManager,
    const Ref<MaterialManager>& materialManager,
    const Ref<ShaderManager>& shaderManager)
{
    NXS_ASSERT(renderingInterface && textureManager && materialManager && shaderManager);
    RegisterLoader(std::make_unique<ModelLoader>(renderingInterface, textureManager, materialManager, shaderManager));
}
