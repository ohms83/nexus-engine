#include "scene/ModelNode.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/MeshComponent.h"

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
    modelComp.SetModel(model);
    for (const auto mesh : model->GetMeshes())
    {
        auto child = EmplaceChild<SceneNode3D>(mesh->GetName());
        child->AddComponent<MeshComponent>()->SetMesh(mesh);
    }
}

void ModelNode::Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface)
{
    Super::Resolve(resourceManager, renderingInterface);

    m_model = resourceManager.Get<Model>(GetComponent<ModelComponent>()->modelPath);
    SetModel(m_model);
}