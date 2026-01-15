#include "graphics/MaterialManager.h"

USING_NAMESPACE_NXS;

MaterialManager::MaterialManager(Ref<RenderingInterface> renderingInterface)
{
    RegisterLoader(std::make_unique<MaterialLoader>());
}