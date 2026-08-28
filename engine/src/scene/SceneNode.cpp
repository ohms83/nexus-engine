#include "scene/SceneNode.h"

#include "core/LogDispatcher.h"
#include "ecs/Component.h"
#include "core/Hasher.h"

#include <utility>
#include <chrono>

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneNode);

static Identifier s_runningId = 0;

std::unordered_map<std::string, SceneNode::Creator> SceneNode::s_factoryFunctions;

SceneNode::SceneNode(Ref<entt::registry> registry, std::string name)
    : Entity(registry)
{
    NXS_ASSERT(s_runningId < MaxIdentifier);
    if (name.empty())
    {
        name = std::format("SceneNode_{}", s_runningId);
    }

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    auto component = AddComponent<SceneNodeComponent>();
    component->id = Hasher().Hash32(std::format("{}_{}_{}", name, milliseconds, s_runningId++));
    component->name = std::move(name);
    component->active = true;
}

SceneNode::~SceneNode()
{
    OnDestroy();
}

void SceneNode::Destroy()
{
    // TODO: With how the scene nodes are passed around as shared pointers, there's no
    // effective way to implement this yet. We need to implement a new memory management first.
    NXS_ASSERT(false);
}

Ref<SceneNode> SceneNode::Create(Ref<entt::registry> registry, std::string className)
{
    if (s_factoryFunctions.find(className) != s_factoryFunctions.end())
    {
        return s_factoryFunctions[className](registry);
    }
    return nullptr;
}

Ref<SceneNode> SceneNode::CreateChild(Ref<SceneNode> parent, std::string className)
{
    if (auto child = Create(parent->GetRegistry(), className); child != nullptr)
    {
        parent->AddChild(child);
        return child;
    }
    return nullptr;
}

void SceneNode::GetRegisteredNodeTypes(std::vector<std::string>& outTypes)
{
    for (const auto& [key, _] : s_factoryFunctions)
    {
        outTypes.push_back(key);
    }
}

void SceneNode::AcceptReflector(IReflector& reflector)
{
    reflector.SetMarker("Properties");

    const auto isActive = IsActive();

    const auto registry = GetRegistry();
    for (const auto id : GetRegisteredComponentIDs())
    {
        if (!IComponent::HasRegisteredID(id)) continue;

        const auto storage = registry->storage(id);
        if (storage && storage->contains(m_entity))
        {
            auto componentPtr = R_CAST<IReflection*>(storage->value(m_entity));
            componentPtr->AcceptReflector(reflector);
        }
    }

    if (isActive != IsActive())
    {
        if (IsActive()) OnActivate();
        else OnDeactivate();
    }
}

VariantData SceneNode::Serialize() const
{
    auto data = VariantData::Map();
    data["__class__"] = ClassName();
    // Node data
    data["children"] = VariantData::Array();
    for (const auto child : m_children)
    {
        data["children"].PushBack(child->Serialize());
    }

    auto& components = data["components"] = VariantData::Array();

    // Component data
    const auto registry = GetRegistry();
    for (const auto id : GetRegisteredComponentIDs())
    {
        if (!IComponent::HasRegisteredType(id)) continue;

        const auto storage = registry->storage(id);
        if (storage && storage->contains(m_entity))
        {
            auto componentPtr = R_CAST<ISerializeable*>(storage->value(m_entity));
            components.PushBack(componentPtr->Serialize());
        }
    }

    return data;
}

bool SceneNode::Deserialize(const VariantData &data)
{
    if (!data.IsMap())
    {
        LOG_ERROR(LogSerialize, std::format("Invalid data type. Type={}", NxsGetTypeString(data.GetType())));
        return false;
    }
    
    if (data["__class__"] != ClassName()) {
        LOG_ERROR(LogSerialize,
            std::format("Unexpected object class. Expected={}, Actual={}", ClassName(), data["__class__"].GetString()));
        return false;
    }

    for (const auto child : data["children"].GetArray())
    {
        const auto className = child["__class__"].GetString();
        if (auto node = CreateChild(GetSelf(), className); node != nullptr)
        {
            if (!node->Deserialize(child))
            {
                LOG_WARNING(LogSerialize, std::format(
                    "Failed to deserialize child node. Parent={}, ChildClass={}",
                    GetName(), className));
            }
        }
        else
        {
            LOG_WARNING(LogSerialize, std::format(
                "Failed to create child node during deserialization. Parent={}, ChildClass={}",
                GetName(), className));
        }
    }

    for (const auto compData : data["components"].GetArray())
    {
        const auto className = compData["__class__"].GetString();
        if (!IComponent::HasRegisteredClassName(className))
        {
            LOG_WARNING(LogSerialize, std::format(
                "Unknown component class during deserialization. Node={}, ComponentClass={}",
                GetName(), className));
            continue;
        }

        auto componentId = IComponent::GetRegisteredTypeID(className);
        IComponent* component = nullptr;
        if (!HasComponent(componentId))
        {
            component = IComponent::AddComponent(className, *this);
        }
        else
        {
            component = GetComponent(componentId);
        }

        if (!component)
        {
            LOG_WARNING(LogSerialize, std::format(
                "Component not found in the entity during deserialization. Node={}, ComponentClass={}",
                GetName(), className));
            continue;
        }
        component->Deserialize(compData);
    }

    return true;
}

void SceneNode::Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface)
{
    std::vector<IComponent*> components;
    GetAllComponents(components);

    for (auto component : components)
    {
        if (!component) continue;
        component->Resolve(resourceManager, renderingInterface);
    }

    for (const auto child : m_children)
    {
        child->Resolve(resourceManager, renderingInterface);
    }

    Validate();

    // Activation callback must be called after all the dependencies are resolved.
    if (IsActive()) OnActivate();
    else OnDeactivate();
}

void SceneNode::Validate()
{
    // Validate all components
    std::vector<IComponent*> components;
    GetAllComponents(components);

    for (auto component : components)
    {
        if (!component) continue;
        component->Validate();
    }

    // Validate all children
    for (const auto child : m_children)
    {
        child->Validate();
    }
}

void SceneNode::Activate(const bool activate)
{
    if (IsActive() != activate)
    {
        GetComponent<SceneNodeComponent>()->active = activate;
        if (activate) OnActivate();
        else OnDeactivate();
    }
}

void SceneNode::AddChild(Ref<SceneNode> child)
{
    child->m_parent = this->GetSelf();
    m_children.push_back(child);
}

void SceneNode::AddChildren(const ChildList& children)
{
    for (const auto& child : children)
    {
        AddChild(child);
    }
}

void SceneNode::RemoveChild(Ref<SceneNode> node, bool removeDescendant)
{
    if (IsShuttingDown() || !node) return;

    SceneNode::ChildList descendants;
    node->GetAllDescendants(descendants, true);
    std::erase(m_children, node);

    // Reparent the children.
    if (!removeDescendant)
    {
        AddChildren(descendants);
    }
}

void SceneNode::RemoveAllChildren(bool removeDescendant)
{
    if (IsShuttingDown()) return;
 
    if (!removeDescendant && m_parent)
    {
        std::ranges::for_each(m_children, [this](Ref<SceneNode> child) {
            m_parent->AddChild(child);
        });
    }
    m_children.clear();
}

void SceneNode::GetAllChildren(ChildList& childrenList) const
{
    for (auto child : m_children) childrenList.push_back(child);
}

void SceneNode::GetAllDescendants(ChildList& childrenList, bool parentFirst) const
{
    for (auto child : m_children)
    {
        if (parentFirst) childrenList.push_back(child);
        else childrenList.insert(childrenList.begin(), child);

        child->GetAllDescendants(childrenList, parentFirst);
    }
}

Ref<SceneNode> SceneNode::FindNode(const Identifier id)
{
    if (IsShuttingDown()) return nullptr;

    if (GetId() == id) return GetSelf();

    for (const auto child : m_children)
    {
        if (child->GetId() == id) return child;
        if (auto node = child->FindNode(id); node != nullptr) return node;
    }

    return nullptr;
}

Ref<SceneNode> SceneNode::FindNodeWithName(const std::string& name)
{
    if (IsShuttingDown()) return nullptr;

    for (const auto child : m_children)
    {
        if (child->GetName() == name) return child;
        if (auto node = child->FindNodeWithName(name); node != nullptr) return node;
    }

    return nullptr;
}

void SceneNode::RemoveFromParent()
{
    if (!m_parent) return;

    std::erase_if(m_parent->m_children, [&](Ref<SceneNode> child) {
        return child.get() == this;
    });
    m_parent = nullptr;
}

void SceneNode::AddScript(Ref<Script> script)
{
    if (auto owner = script->GetOwner(); owner != nullptr) owner->RemoveScript(script);

    script->SetOwner(GetSelf());
    m_scripts.push_back(script);

    std::ranges::sort(m_scripts, std::ranges::greater{}, &Script::GetPriority);
}

void SceneNode::RemoveScript(Ref<Script> script)
{
    if (!script || !script->GetOwner()) return;

    std::erase(m_scripts, script);
}

void SceneNode::Update(float dt)
{
    std::ranges::for_each(m_scripts, [dt](Ref<Script> script) {
        script->Update(dt);
    });

    auto& registry = *GetRegistry();
    std::ranges::for_each(m_simulations, [dt, &registry](Simulation& sim) {
        sim.system(registry, dt);
    });

    std::ranges::for_each(m_children, [dt](Ref<SceneNode> node) {
        node->Update(dt);
    });

    OnUpdate(dt);
}

uint32_t SceneNode::AddSimulation(ECS::SimulationSystem system)
{
    static uint32_t runningNumber = 0;
    m_simulations.push_back({ runningNumber, system });
    return runningNumber++;
}

void SceneNode::RemoveSimulation(uint32_t id)
{
    if (auto itr = std::ranges::find(m_simulations, id, &Simulation::id); itr != m_simulations.end()) {
        m_simulations.erase(itr);
    }
}
