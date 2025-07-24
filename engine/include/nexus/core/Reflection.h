//
// Created by nutta on 7/23/2025.
//

#pragma once

#include <string>

#include "nexus/NxsDefine.h"

#define IMPLEMENT_REFLECTION(Class) \
    const std::string& ClassName() const override { static std::string className = #Class; return className; } \
    static const std::string& StaticClassName() { static std::string className = #Class; return className; } \

NXS_NAMESPACE
{
    class IReflection
    {
    public:
        virtual ~IReflection() {}
        virtual const std::string& ClassName() const = 0;
    };
}
