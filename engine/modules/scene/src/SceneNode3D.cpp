#include "nexus/scene/SceneNode3D.h"
#include "nexus/math/Matrix.h"

USING_NAMESPACE_NXS;

SceneNode3D::SceneNode3D(Ref<entt::registry> registry, std::string name)
    : SceneNode(registry, std::move(name))
{
    AddComponents<PositionComponent, OrientationComponent, ScaleComponent>();
}
