#include "scene/MeshNode.h"

USING_NAMESPACE_NXS;

MeshNode::MeshNode(Ref<entt::registry> registry, const std::string& name)
    : SceneNode3D(registry, name)
{
    AddComponent<MeshComponent>();
}