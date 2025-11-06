#include "scene/MeshNode.h"

USING_NAMESPACE_NXS;

MeshNode::MeshNode(Ref<entt::registry> registry, const std::string& name)
    : SceneNode3D(registry, name)
{
    AddComponent<MeshComponent>();
}

void MeshNode::AcceptReflector(IReflector& reflector)
{
    SceneNode3D::AcceptReflector(reflector);

    auto& meshComp = GetComponent<MeshComponent>();
    reflector.ChangeCatetory("Mesh");
    reflector.VisitPropertyWithFeedback("Name", typeid(std::string), (void*)(meshComp.mesh->GetName().c_str()),  [](void* newValue) {
        // TODO:
    });
    reflector.VisitProperty("Show Bounding Sphere", typeid(bool), &meshComp.showBoundingSphere);
    reflector.VisitProperty("Show Bounding Box", typeid(bool), &meshComp.showBoundingBox);
}