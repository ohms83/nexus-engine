#include "graphics/MaterialManager.h"

USING_NAMESPACE_NXS;

MaterialManager::MaterialManager(Ref<RenderingInterface> renderingInterface)
{
    RegisterLoader(std::make_unique<MaterialLoader>());

    // Create default material
    m_defaultMaterial = std::make_shared<Material>("DefaultMaterial", 0);
    m_defaultMaterial->CreateDefaultShader(renderingInterface);
}