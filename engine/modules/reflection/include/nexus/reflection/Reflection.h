//
// Created by nutta on 7/23/2025.
//

#pragma once

#include <string>
#include <typeindex>
#include <type_traits>
#include <concepts>

#include "nexus/NxsCommon.h"
#include "nexus/serialize/Serializeable.h"
#include "nexus/core/Color.h"

#include "sigslot/signal.hpp"

#define IMPLEMENT_REFLECTION(Class) \
public: \
    const std::string& ClassName() const override { static std::string className = #Class; return className; } \
    static const std::string& StaticClassName() { static std::string className = #Class; return className; } \

#define IMPLEMENT_REFLECTION_STRUCT(Struct) \
    const std::string& ClassName() const override { static std::string className = #Struct; return className; } \
    static const std::string& StaticClassName() { static std::string className = #Struct; return className; } \

NXS_NAMESPACE
{
    class IReflection;

    class IReflector
    {
    public:
        virtual ~IReflector() = default;
        /**
         * @brief Set a string marker. This can be used for property separation.
         * 
         * @param name Marker name.
         */
        virtual void SetMarker(const std::string& name) = 0;

        /**
         * @brief Mark the subsequent visit functions to operate in read-only mode.
         * 
         * @param value If @c true, the subsequent visit functions will operate in read-only mode.
         * @c false, otherwise.
         */
        virtual void SetReadOnlyFlag(bool value) = 0;

        MAYBE_UNUSED bool VisitBool(const std::string& name, bool& value)
        {
            if (VisitBool_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitInt(const std::string& name, int32_t& value)
        {
            if (VisitInt_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitUInt(const std::string& name, uint32_t& value)
        {
            if (VisitUInt_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitInt64(const std::string& name, int64_t& value)
        {
            if (VisitInt64_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitUInt64(const std::string& name, uint64_t& value)
        {
            if (VisitUInt64_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitFloat(const std::string& name, float& value)
        {
            if (VisitFloat_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitDouble(const std::string& name, double& value)
        {
            if (VisitDouble_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitString(const std::string& name, std::string& value)
        {
            if (VisitString_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitVec2(const std::string& name, glm::vec2& value)
        {
            if (VisitVec2_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitVec3(const std::string& name, glm::vec3& value)
        {
            if (VisitVec3_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitVec4(const std::string& name, glm::vec4& value)
        {
            if (VisitVec4_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitColor3(const std::string& name, Color3F& value)
        {
            if (VisitColor3_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitColor4(const std::string& name, Color4F& value)
        {
            if (VisitColor4_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitObject(const std::string& name, IReflection& value)
        {
            if (VisitObject_Implement(name, value)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }
        MAYBE_UNUSED bool VisitEnum(const std::string& name, int32_t& value, const Array<std::string>& enumStrings)
        {
            if (VisitEnum_Implement(name, value, enumStrings)) {
                onValueChangedEvent(name);
                return true;
            }
            return false;
        }

        template<typename T>
        MAYBE_UNUSED bool Visit(const std::string& name, T& value)
        {
            #define VISIT(Type) if (Visit##Type##_Implement(name, value)) { onValueChangedEvent(name); return true; }

            if constexpr (std::is_same_v<T, bool>)
            {
                VISIT(Bool);
            }
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                VISIT(Int);
            }
            else if constexpr (std::is_same_v<T, uint32_t>)
            {
                VISIT(UInt);
            }
            else if constexpr (std::is_same_v<T, int64_t>)
            {
                VISIT(Int64);
            }
            else if constexpr (std::is_same_v<T, uint64_t>)
            {
                VISIT(UInt64);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                VISIT(Float);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                VISIT(Double);
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                VISIT(String);
            }
            else if constexpr (std::is_same_v<T, glm::vec2>)
            {
                VISIT(Vec2);
            }
            else if constexpr (std::is_same_v<T, glm::vec3>)
            {
                VISIT(Vec3);
            }
            else if constexpr (std::is_same_v<T, glm::vec4>)
            {
                VISIT(Vec4);
            }
            else if constexpr (std::is_same_v<T, Color3F>)
            {
                VISIT(Color3);
            }
            else if constexpr (std::is_same_v<T, Color4F>)
            {
                VISIT(Color4);
            }
            else if constexpr (std::is_base_of_v<IReflection, T>)
            {
                VISIT(Object);
            }
            else {
                static_assert(false);
            }
            #undef VISIT
            return false;
        }

        sigslot::signal<const std::string&> onValueChangedEvent;

    private:
        NODISCARD virtual bool VisitBool_Implement(const std::string& name, bool& value) = 0;
        NODISCARD virtual bool VisitInt_Implement(const std::string& name, int32_t& value) = 0;
        NODISCARD virtual bool VisitUInt_Implement(const std::string& name, uint32_t& value) = 0;
        NODISCARD virtual bool VisitInt64_Implement(const std::string& name, int64_t& value) = 0;
        NODISCARD virtual bool VisitUInt64_Implement(const std::string& name, uint64_t& value) = 0;
        NODISCARD virtual bool VisitFloat_Implement(const std::string& name, float& value) = 0;
        NODISCARD virtual bool VisitDouble_Implement(const std::string& name, double& value) = 0;
        NODISCARD virtual bool VisitString_Implement(const std::string& name, std::string& value) = 0;
        NODISCARD virtual bool VisitVec2_Implement(const std::string& name, glm::vec2& value) = 0;
        NODISCARD virtual bool VisitVec3_Implement(const std::string& name, glm::vec3& value) = 0;
        NODISCARD virtual bool VisitVec4_Implement(const std::string& name, glm::vec4& value) = 0;
        NODISCARD virtual bool VisitColor3_Implement(const std::string& name, Color3F& value) = 0;
        NODISCARD virtual bool VisitColor4_Implement(const std::string& name, Color4F& value) = 0;
        NODISCARD virtual bool VisitObject_Implement(const std::string& name, IReflection& value) = 0;
        NODISCARD virtual bool VisitEnum_Implement(const std::string& name, int32_t& value, const Array<std::string>& enumStrings) = 0;
    };

    class IReflection : public ISerializeable
    {
    public:
        virtual ~IReflection() = default;
        NODISCARD virtual const std::string& ClassName() const = 0;

        virtual void AcceptReflector(IReflector& reflector) = 0;
    };
}
