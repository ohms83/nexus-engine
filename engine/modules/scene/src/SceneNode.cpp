#include "nexus/scene/SceneNode.h"
#include "nexus/container/Array.h"

#include "nexus/debug/LogDispatcher.h"
#include "nexus/ecs/Component.h"
#include "nexus/core/Hasher.h"
#include "nexus/scene/component/TransformComponent.h"

#include <utility>
#include <chrono>

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneNode);

static Identifier s_runningId = 0;

HashMap<std::string, SceneNode::Creator> SceneNode::s_factoryFunctions;
HashMap<Identifier, SceneNode*> SceneNode::s_nodeRegistry;

SceneNode::SceneNode(Ref<entt::registry> registry, std::string name)
    : Entity(registry)
{
    NXS_ASSERT(s_runningId < NXS_MAX_IDENTIFIER);
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

    s_nodeRegistry.emplace(component->id, this);
}

SceneNode::~SceneNode()
{
    OnDestroy();
    s_nodeRegistry.erase(GetId());
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

void SceneNode::GetRegisteredNodeTypes(Array<std::string>& outTypes)
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
    Array<IComponent*> components;
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
    Array<IComponent*> components;
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
    rmt_ScopedCPUSample(SceneNode_AddChild, 0);
    if (!child) return;

    const auto self = GetSelf();
    for (auto ancestor = self; ancestor; ancestor = ancestor->m_parent.lock())
    {
        if (ancestor == child)
        {
            LOG_WARNING(LogSceneNode, "Cannot add a scene node as its own ancestor.");
            return;
        }
    }

    const auto previousParent = child->m_parent.lock();
    if (previousParent == self) return;
    if (previousParent)
    {
        std::erase(previousParent->m_children, child);
    }

    child->m_parent = self;
    m_children.push_back(child);
    child->UpdateGlobalTransform();
}

void SceneNode::SetPosition(const glm::vec3& position)
{
    rmt_ScopedCPUSample(SceneNode_SetPosition, 0);
    auto positionComponent = GetComponent<PositionComponent>();
    if (!positionComponent) return;
    positionComponent->value = position;
    m_localTransformExplicit = false;
    m_transformDirty = true;
}

void SceneNode::SetOrientation(const glm::quat& orientation)
{
    rmt_ScopedCPUSample(SceneNode_SetOrientation, 0);
    auto orientationComponent = GetComponent<OrientationComponent>();
    if (!orientationComponent) return;
    orientationComponent->quat = orientation;
    m_localTransformExplicit = false;
    m_transformDirty = true;
}

void SceneNode::SetScale(const glm::vec3& scale)
{
    rmt_ScopedCPUSample(SceneNode_SetScale, 0);
    auto scaleComponent = GetComponent<ScaleComponent>();
    if (!scaleComponent) return;
    scaleComponent->value = scale;
    m_localTransformExplicit = false;
    m_transformDirty = true;
}

void SceneNode::SetLocalTransform(const glm::mat4& transform)
{
    rmt_ScopedCPUSample(SceneNode_LocalTransform, 0);
    m_transform = transform;
    m_localTransformExplicit = true;
    m_transformDirty = true;
}

const glm::vec3& SceneNode::GetPosition() const
{
    auto positionComponent = GetComponent<PositionComponent>();
    static const glm::vec3 defaultPosition(0.0f);
    return positionComponent ? positionComponent->value : defaultPosition;
}

glm::quat SceneNode::GetOrientation() const
{
    auto orientationComponent = GetComponent<OrientationComponent>();
    return orientationComponent ? orientationComponent->quat : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

glm::vec3 SceneNode::GetScale() const
{
    auto scaleComponent = GetComponent<ScaleComponent>();
    return scaleComponent ? scaleComponent->value : glm::vec3(1.0f);
}

const glm::mat4& SceneNode::GetLocalTransform() const
{
    UpdateTransform();
    return m_transform;
}

void SceneNode::UpdateTransform() const
{
    rmt_ScopedCPUSample(SceneNode_UpdateTransform, 0);
    if (!m_transformDirty) return;

    if (!m_localTransformExplicit)
    {
        const auto translationMatrix = glm::translate(glm::mat4(1.0f), GetPosition());
        const auto rotationMatrix = glm::toMat4(GetOrientation());
        const auto scaleMatrix = glm::scale(glm::mat4(1.0f), GetScale());
        m_transform = translationMatrix * rotationMatrix * scaleMatrix;
    }

    m_transformDirty = false;
}

void SceneNode::UpdateGlobalTransform()
{
    rmt_ScopedCPUSample(SceneNode_UpdateGlobalTransform, 0);
    m_transformDirty = true;
    UpdateTransform();
    for (const auto& child : m_children)
    {
        child->UpdateGlobalTransform();
    }
}

const glm::mat4& SceneNode::GetGlobalTransform() const
{
    rmt_ScopedCPUSample(SceneNode_GetGlobalTransform, 0);
    UpdateTransform();
    auto parent = m_parent.lock();
    m_globalTransform = (parent ? parent->GetGlobalTransform() * m_transform : m_transform);
    return m_globalTransform;
}

glm::vec3 SceneNode::GetGlobalPosition() const
{
    rmt_ScopedCPUSample(SceneNode_GetGlobalPosition, 0);
    return glm::vec3(GetGlobalTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::quat SceneNode::GetGlobalOrientation() const
{
    rmt_ScopedCPUSample(SceneNode_GetGlobalOrientation, 0);
    const auto orientation = GetComponent<OrientationComponent>();
    const auto localOrientation = orientation
        ? orientation->quat
        : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    const auto parent = m_parent.lock();
    return parent ? parent->GetGlobalOrientation() * localOrientation : localOrientation;
}

glm::vec3 SceneNode::GetGlobalScale() const
{
    rmt_ScopedCPUSample(SceneNode_GetGlobalScale, 0);
    const auto globalTransform = GetGlobalTransform();
    return {
        glm::length(glm::vec3(globalTransform[0])),
        glm::length(glm::vec3(globalTransform[1])),
        glm::length(glm::vec3(globalTransform[2]))
    };
}

void SceneNode::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    if (auto orientationComponent = GetComponent<OrientationComponent>())
    {
        orientationComponent->LookAt(GetPosition(), center, up);
        m_localTransformExplicit = false;
        m_transformDirty = true;
    }
}

glm::vec3 SceneNode::Right() const
{
    static const glm::vec3 right = {1, 0, 0};
    return GetOrientation() * right;
}

glm::vec3 SceneNode::Up() const
{
    static const glm::vec3 up = {0, 1, 0};
    return GetOrientation() * up;
}

glm::vec3 SceneNode::Forward() const
{
    static const glm::vec3 forward = {0, 0, -1};
    return GetOrientation() * forward;
}

void SceneNode::AddChildren(const ChildList& children)
{
    std::ranges::for_each(children, [&](Ref<SceneNode> child) {
        AddChild(child);
    });
}

void SceneNode::RemoveChild(Ref<SceneNode> node, bool reparentDescendants)
{
    rmt_ScopedCPUSample(SceneNode_RemoveChild, 0);
    if (IsShuttingDown() || !node) return;

    const size_t numRemoved = std::erase_if(m_children, [&](const Ref<SceneNode>& child) {
        return child == node;
    });

    if (numRemoved == 0)
    {
        LOG_WARNING(LogSceneNode, std::format("Failed to remove node '{}' from its parent '{}'.", node->GetName(), GetName()));
        return;
    }

    SceneNode::ChildList children;
    node->m_parent.reset();
    node->UpdateGlobalTransform();

    if (reparentDescendants)
    {
        node->GetAllChildren(children);
        AddChildren(children);
    }
}

void SceneNode::RemoveAllChildren(bool removeDescendant)
{
    if (IsShuttingDown()) return;

    const auto parent = m_parent.lock();
    auto children = std::move(m_children);
    m_children.clear();
    if (!removeDescendant && parent)
    {
        parent->AddChildren(children);
    }
    else
    {
        for (const auto& child : children)
        {
            child->m_parent.reset();
            child->UpdateGlobalTransform();
        }
    }
}

void SceneNode::GetAllChildren(ChildList& childrenList) const
{
    rmt_ScopedCPUSample(SceneNode_GetAllChildren, 0);
    for (auto child : m_children) childrenList.push_back(child);
}

void SceneNode::GetAllDescendants(ChildList& childrenList, bool parentFirst) const
{
    rmt_ScopedCPUSample(SceneNode_GetAllDescendants, 0);
    for (auto child : m_children)
    {
        if (parentFirst) childrenList.push_back(child);
        else childrenList.insert(childrenList.begin(), child);

        child->GetAllDescendants(childrenList, parentFirst);
    }
}

Ref<SceneNode> SceneNode::FindChild(const Identifier id, bool recursive)
{
    rmt_ScopedCPUSample(SceneNode_FindChild, 0);
    if (IsShuttingDown()) return nullptr;

    if (GetId() == id) return GetSelf();

    for (const auto child : m_children)
    {
        if (child->GetId() == id) return child;
        if (recursive)
        {
            if (auto node = child->FindChild(id, true); node != nullptr) return node;
        }
    }

    return nullptr;
}

NODISCARD SceneNode* SceneNode::FindNode(Identifier id)
{
    rmt_ScopedCPUSample(SceneNode_FindNode, 0);
    auto it = s_nodeRegistry.find(id);
    return (it != s_nodeRegistry.end()) ? it->second : nullptr;
}

Ref<SceneNode> SceneNode::FindNodeWithName(const std::string& name)
{
    rmt_ScopedCPUSample(SceneNode_FindNodeWithName, 0);
    if (IsShuttingDown()) return nullptr;

    for (const auto child : m_children)
    {
        if (child->GetName() == name) return child;
        if (auto node = child->FindNodeWithName(name); node != nullptr) return node;
    }

    return nullptr;
}

void SceneNode::RemoveFromParent(bool reparentDescendants)
{
    if (const auto parent = m_parent.lock())
    {
        parent->RemoveChild(GetSelf(), reparentDescendants);
    }
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
    rmt_ScopedCPUSample(SceneNode_Update, 0);
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
