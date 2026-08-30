#pragma once

#include "nexus/reflection/Reflection.h"
#include "EcsDefine.h"
#include "Entity.h"

#include <set>
#include <map>
#include <string>

#define COMPONENT_HASH(Type) entt::type_id<Type>().hash()
#define IMPLEMENT_COMPONENT(Type) \
    IMPLEMENT_REFLECTION(Type); \
    static ComponentID StaticComponentID() { \
        return COMPONENT_HASH(Type); \
    } \
    ComponentID GetComponentID() const override { \
        return StaticComponentID(); \
    } \
    static void Register() { \
        s_componentTypes[#Type] = COMPONENT_HASH(Type); \
        s_componentFactory[#Type] = [](Entity& entity) { \
            return CAST<IComponent*>(entity.AddComponent<Type>()); \
        }; \
    }

NXS_NAMESPACE
{
    using ComponentID = entt::id_type;

    class RenderingInterface;
    class ResourceManager;

    class IComponent : public IReflection
    {
    public:
        /**
         * @brief A helper function used for self registering a component.
         */
        using ComponentRegister = std::function<IComponent*(Entity&)>;

        virtual ~IComponent() = default;
        virtual ComponentID GetComponentID() const = 0;

        NODISCARD VariantData Serialize() const override
        {
            auto data = VariantData::Map();
            data["__class__"] = ClassName();
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (data["__class__"].GetString() != ClassName())
            {
                LOG_ERROR(LogSerialize, std::format("Unexpected object class. Expected={}, Actual={}",
                    ClassName(), data["__class__"].GetString()));
                return false;
            }
            return true;
        }

        /**
         * @brief Validate and clamp component properties. This should be performed
         * after deserialization to ensure data integrity (eg. divide by zero prevention).
         * @note This function is optional to implement in derived classes.
         */
        virtual void Validate() {}

        /**
         * @brief Resolve object's dependencies. This must be called after `Deserialization`.
         * 
         * @param resourceManager The resource manager used to resolve resources.
         * @param renderingInterface Rendering interface used to resolve graphical components.
         * @note This function is optional to implement in derived classes.
         */
        virtual void Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface) {}
        
        template<typename Type>
        NODISCARD static bool HasRegisteredType()
        {
            return HasRegisteredID(COMPONENT_HASH(Type));
        }

        NODISCARD static bool HasRegisteredType(const ComponentID type_id)
        {
            return std::ranges::any_of(s_componentTypes, [&type_id](const auto& pair) {
                return pair.second == type_id;
            });
        }

        NODISCARD static bool HasRegisteredClassName(const std::string& className)
        {
            return s_componentFactory.contains(className);
        }

        /**
         * @brief Get the registered ComponentID for a given class name.
         * 
         * @param className 
         * @return A valid ComponentID if found; InvalidID otherwise.
         */
        NODISCARD static ComponentID GetRegisteredTypeID(const std::string& className)
        {
            if (auto itr = s_componentTypes.find(className); itr != s_componentTypes.end())
            {
                return itr->second;
            }
            return InvalidID;
        }

        /**
         * @brief Adds a component to an entity by its registered name.
         * 
         * This function looks up the component's name in the component factory.
         * If a corresponding component registration is found, it is used to
         * add the component to the specified entity.
         * 
         * @param name The registered name of the component to add.
         * @param entity The entity to which the component will be added.
         * @return A pointer to the added component if successful, @c nullptr otherwise.
         */
        MAYBE_UNUSED static IComponent* AddComponent(const std::string& name, Entity& entity)
        {
            if (auto itr = s_componentFactory.find(name); itr != s_componentFactory.end())
            {
                return itr->second(entity);
            }
            return nullptr;
        }

        NODISCARD static bool HasRegisteredID(const ComponentID type_id)
        {
            return std::ranges::any_of(s_componentTypes, [&type_id](const auto& pair) {
                return pair.second == type_id;
            });
        }
    
    protected:
        static std::map<std::string, ComponentID> s_componentTypes;
        static std::map<std::string, ComponentRegister> s_componentFactory;
    };
}
