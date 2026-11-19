#pragma once

#include "nexus/Nexus.h"

class FakeReflector : public nxs::IReflector
{
public:
    void SetMarker(const std::string&) override {}
    void SetReadOnlyFlag(bool) override {}
    bool VisitBool(const std::string& name, bool& value) { return false; }
    bool VisitInt(const std::string& name, int32_t& value) { return false; }
    bool VisitUInt(const std::string& name, uint32_t& value) { return false; }
    bool VisitInt64(const std::string& name, int64_t& value) { return false; }
    bool VisitUInt64(const std::string& name, uint64_t& value) { return false; }
    bool VisitFloat(const std::string& name, float& value) { return false; }
    bool VisitDouble(const std::string& name, double& value) { return false; }
    bool VisitString(const std::string& name, std::string& value) { return false; }
    bool VisitVec2(const std::string& name, glm::vec2& value) { return false; }
    bool VisitVec3(const std::string& name, glm::vec3& value) { return false; }
    bool VisitVec4(const std::string& name, glm::vec4& value) { return false; }
    bool VisitColor3(const std::string& name, nxs::Color3F& value) { return false; }
    bool VisitColor4(const std::string& name, nxs::Color4F& value) { return false; }
    bool VisitObject(const std::string& name, nxs::IReflection& value) { return false; }
    bool VisitEnum(const std::string& name, int32_t& value, const nxs::Array<std::string>& enumStrings) { return false; }
};