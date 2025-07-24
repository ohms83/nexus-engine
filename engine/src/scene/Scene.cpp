#include "nexus/scene/Scene.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Ref<SceneNode> Scene::CreateNode(const std::string& name, Ref<SceneNode> parent)
{
    auto& node = m_children.emplace_back(std::make_shared<SceneNode>(name));
    if (parent) {
        parent->transform.AddChild(&node->transform);
    }
    return node;
}

Ref<SceneNode> Scene::CreateNode(const std::string &name, const std::string &parentName)
{
    auto itr = std::ranges::find_if(m_children, [parentName](Ref<SceneNode> node) {
        return node->GetName() == parentName;
    });

    Ref<SceneNode> parent;
    if (itr == m_children.end()) {
        LOG_WARNING(LogScene, std::format("Cannot find a parent node {}", parentName));
    }
    else {
        parent = *itr;
    }
    return CreateNode(name, parent);
}

void Scene::Update()
{
}

void Scene::Render(RenderSystem &renderSystem)
{
}
