//
// Created by nutta on 7/23/2025.
//

#pragma once

#include <string>
#include <typeindex>

#include "nexus/NxsCommon.h"
#include "nexus/serialize/Serializeable.h"
#include "Color.h"

#include "sigslot/signal.hpp"

#define IMPLEMENT_REFLECTION(Class) \
public: \
    const std::string& ClassName() const override { static std::string className = #Class; return className; } \
    static const std::string& StaticClassName() { static std::string className = #Class; return className; } \

NXS_NAMESPACE
{
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

        virtual bool VisitBool(const std::string& name, bool& value) = 0;
        virtual bool VisitInt(const std::string& name, int32_t& value) = 0;
        virtual bool VisitUInt(const std::string& name, uint32_t& value) = 0;
        virtual bool VisitInt64(const std::string& name, int64_t& value) = 0;
        virtual bool VisitUInt64(const std::string& name, uint64_t& value) = 0;
        virtual bool VisitFloat(const std::string& name, float& value) = 0;
        virtual bool VisitDouble(const std::string& name, double& value) = 0;
        virtual bool VisitString(const std::string& name, std::string& value) = 0;
        virtual bool VisitVec2(const std::string& name, glm::vec2& value) = 0;
        virtual bool VisitVec3(const std::string& name, glm::vec3& value) = 0;
        virtual bool VisitVec4(const std::string& name, glm::vec4& value) = 0;
        virtual bool VisitColor3(const std::string& name, Color3F& value) = 0;
        virtual bool VisitColor4(const std::string& name, Color4F& value) = 0;
        virtual bool VisitObject(const std::string& name, IReflector& value) = 0;

        template<typename T>
        bool Visit(const std::string& name, T& value)
        {
            #define VISIT(Type) if (Visit##Type(name, value)) { onValueChangedEvent(name); return true; }

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
            else if constexpr (std::is_base_of_v<IReflector, T>)
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
    };

    class IReflection : public ISerializeable
    {
    public:
        virtual ~IReflection() {}
        virtual const std::string& ClassName() const = 0;

        virtual void AcceptReflector(IReflector& reflector) = 0;
    };
}
