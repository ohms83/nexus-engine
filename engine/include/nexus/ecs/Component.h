#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Reflection.h"

#include "entt/entt.hpp"
#include <set>

NXS_NAMESPACE
{
    class IComponent : public IReflection
    {
    public:
        virtual ~IComponent() = default;
        virtual entt::id_type GetComponentID() const = 0;
    
    private:
        static std::set<entt::id_type> s_componentTypes;
    };
}