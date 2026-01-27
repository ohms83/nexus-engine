#include "scene/ModelNode.h"

#include <format>
#include <filesystem>

USING_NAMESPACE_NXS;

static uint32_t s_numNode = 0;

ModelNode::ModelNode(Ref<entt::registry> registry, std::string name)
    : SceneNode3D(registry, std::move(name))
{
    AddComponent<ModelComponent>();
}

void ModelNode::SetModel(Ref<Model> model)
{
    ModelComponent& modelComp = *GetComponent<ModelComponent>();
    modelComp.model = model;
    for (const auto mesh : model->GetMeshes())
    {
        auto child = EmplaceChild<SceneNode3D>(mesh->GetName());
        child->AddComponent<MeshComponent>()->mesh = mesh;
    }
}