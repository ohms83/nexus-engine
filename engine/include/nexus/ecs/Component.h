#pragma once

#include "nexus/core/Reflection.h"

#include "entt/entt.hpp"
#include <set>

#define COMPONENT_HASH(Type) entt::type_id<Type>().hash()

NXS_NAMESPACE
{
    class IComponent : public IReflection
    {
    public:
        using ComponentID = entt::id_type;

        virtual ~IComponent() = default;
        virtual ComponentID GetComponentID() const = 0;
        
        template<typename Type>
        static bool HasRegistered()
        {
            return s_componentTypes.find(COMPONENT_HASH(Type)) != s_componentTypes.end();
        }

    protected:
        template<typename Type>
        static void RegisterComponent()
        {
            s_componentTypes.insert(COMPONENT_HASH(Type));
        }
    
    private:
        static std::set<ComponentID> s_componentTypes;
    };
}
