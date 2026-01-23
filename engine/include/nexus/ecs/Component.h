#pragma once

#include "nexus/core/Reflection.h"
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
        s_componentTypes.insert(COMPONENT_HASH(Type)); \
        s_componentFactory[#Type] = [](Entity& entity) { \
            entity.AddComponent<Type>(); \
        }; \
    }

NXS_NAMESPACE
{
    using ComponentID = entt::id_type;
    /**
     * @brief A helper function used for self registering a component.
     */
    using ComponentRegister = std::function<void(Entity&)>;

    class IComponent : public IReflection
    {
    public:
        virtual ~IComponent() = default;
        virtual ComponentID GetComponentID() const = 0;

        VariantData Serialize() const override
        {
            auto data = VariantData::Map();
            data["__class__"] = ClassName();
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (data["__class__"].GetString() == ClassName())
            {
                LOG_ERROR(LogSerialize, std::format("Unexpected object class. Expected={}, Actual={}",
                    ClassName(), data["__class__"].GetString()));
                return false;
            }
            return true;
        }
        
        template<typename Type>
        static bool HasRegisteredType()
        {
            return s_componentTypes.find(COMPONENT_HASH(Type)) != s_componentTypes.end();
        }

        static bool HasRegisteredID(const ComponentID id)
        {
            return s_componentTypes.find(id) != s_componentTypes.end();
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
         * @return @c true if the component was successfully found and added, @c false otherwise.
         */
        MAYBE_UNUSED static bool AddComponent(const std::string& name, Entity& entity)
        {
            if (auto itr = s_componentFactory.find(name); itr != s_componentFactory.end())
            {
                itr->second(entity);
                return true;
            }
            return false;
        }
    
    protected:
        static std::set<ComponentID> s_componentTypes;
        static std::map<std::string, ComponentRegister> s_componentFactory;
    };
}
