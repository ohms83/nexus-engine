#pragma once

#include "nexus/core/Reflection.h"
#include "EcsDefine.h"

#include <set>

#define COMPONENT_HASH(Type) entt::type_id<Type>().hash()
#define IMPLEMENT_COMPONENT(Type) \
    IMPLEMENT_REFLECTION(Type); \
    ComponentID GetComponentID() const override \
    { \
        return COMPONENT_HASH(Type); \
    }

NXS_NAMESPACE
{
    using ComponentID = entt::id_type;

    class IComponent : public IReflection
    {
    public:
        virtual ~IComponent() = default;
        virtual ComponentID GetComponentID() const = 0;
        
        template<typename Type>
        static bool HasRegisteredType()
        {
            return s_componentTypes.find(COMPONENT_HASH(Type)) != s_componentTypes.end();
        }

        static bool HasRegisteredID(const ComponentID id)
        {
            return s_componentTypes.find(id) != s_componentTypes.end();
        }

        template<typename Type>
        static void RegisterComponent()
        {
            s_componentTypes.insert(COMPONENT_HASH(Type));
        }
    
    private:
        static std::set<ComponentID> s_componentTypes;
    };
}
