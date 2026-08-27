//
// Created by nutta on 7/12/2025.
//

#include "graphics/MaterialLoader.h"

USING_NAMESPACE_NXS;

MaterialLoader::MaterialLoader(const Ref<RenderingInterface>& renderingInterface, ResourceManager& resourceManager)
    : m_renderingInterface(renderingInterface)
    , m_resourceManager(resourceManager)
{
}

Ref<Resource> MaterialLoader::Load(const std::string& path, uint32 id)
{
    return nullptr;
}
