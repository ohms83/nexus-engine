#include "scene/ModelNode.h"
#include "scene/MeshNode.h"

#include <format>
#include <filesystem>

USING_NAMESPACE_NXS;

static uint32_t s_numNode = 0;

ModelNode::ModelNode(Ref<entt::registry> registry)
    : SceneNode3D(registry, std::format("Model_{}", s_numNode++))
{
    AddComponent<ModelComponent>();
}

ModelNode::ModelNode(Ref<entt::registry> registry, const std::string& name)
    : SceneNode3D(registry, name)
{
    AddComponent<ModelComponent>();
}

ModelNode::ModelNode(Ref<entt::registry> registry, Ref<Model> model)
    : SceneNode3D(registry)
{
    AddComponent<ModelComponent>(model);

    auto path = std::filesystem::path(model->GetPath());
    SetName(path.filename().string());
    SetModel(model);
}

void ModelNode::AcceptReflector(IReflector& reflector)
{
    SceneNode3D::AcceptReflector(reflector);

    reflector.ChangeCatetory("Model");

    auto& modelComp = GetComponent<ModelComponent>();
    auto model = modelComp.model;
    auto path = model->GetPath();

    reflector.VisitPropertyWithFeedback("Path", typeid(std::string), (void*)(path.c_str()), [&path](void* newValue) {
        // TODO:
    });
    reflector.VisitProperty("Show Bounding Sphere", typeid(bool), &modelComp.showBoundingSphere);
    reflector.VisitProperty("Show Bounding Box", typeid(bool), &modelComp.showBoundingBox);
}

void ModelNode::SetModel(Ref<Model> model)
{
    ModelComponent& modelComp = GetComponent<ModelComponent>();
    modelComp.model = model;
    for (const auto mesh : model->GetMeshes())
    {
        auto child = EmplaceChild<MeshNode>(mesh->GetName());
        child->GetComponent<MeshComponent>().mesh = mesh;
    }
}