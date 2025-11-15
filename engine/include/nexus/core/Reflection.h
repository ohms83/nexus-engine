//
// Created by nutta on 7/23/2025.
//

#pragma once

#include <string>
#include <typeindex>

#include "nexus/NxsDefine.h"

#define IMPLEMENT_REFLECTION(Class) \
    const std::string& ClassName() const override { static std::string className = #Class; return className; } \
    static const std::string& StaticClassName() { static std::string className = #Class; return className; } \

NXS_NAMESPACE
{
    class IReflector
    {
    public:
        virtual void ChangeCategory(const std::string& name) = 0;
        virtual void VisitPropertyWithFeedback(const std::string& name, std::type_index type, void* value, std::function<void(void*)> callback) = 0;
        
        virtual void VisitProperty(const std::string& name, std::type_index type, void* value)
        {
            VisitPropertyWithFeedback(name, type, value, [](void*){});
        }
    };

    class IReflection
    {
    public:
        virtual ~IReflection() {}
        virtual const std::string& ClassName() const = 0;

        virtual void AcceptReflector(IReflector& reflector) = 0;
    };
}
