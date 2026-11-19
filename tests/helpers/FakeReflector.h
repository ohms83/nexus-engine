#pragma once

#include "nexus/Nexus.h"

class FakeReflector : public nxs::IReflector
{
public:
    void SetMarker(const std::string&) override {}
    void SetReadOnlyFlag(bool) override {}

private:
    bool VisitBool_Implement(const std::string& name, bool& value) { return false; }
    bool VisitInt_Implement(const std::string& name, int32_t& value) { return false; }
    bool VisitUInt_Implement(const std::string& name, uint32_t& value) { return false; }
    bool VisitInt64_Implement(const std::string& name, int64_t& value) { return false; }
    bool VisitUInt64_Implement(const std::string& name, uint64_t& value) { return false; }
    bool VisitFloat_Implement(const std::string& name, float& value) { return false; }
    bool VisitDouble_Implement(const std::string& name, double& value) { return false; }
    bool VisitString_Implement(const std::string& name, std::string& value) { return false; }
    bool VisitVec2_Implement(const std::string& name, glm::vec2& value) { return false; }
    bool VisitVec3_Implement(const std::string& name, glm::vec3& value) { return false; }
    bool VisitVec4_Implement(const std::string& name, glm::vec4& value) { return false; }
    bool VisitColor3_Implement(const std::string& name, nxs::Color3F& value) { return false; }
    bool VisitColor4_Implement(const std::string& name, nxs::Color4F& value) { return false; }
    bool VisitObject_Implement(const std::string& name, nxs::IReflection& value) { return false; }
    bool VisitEnum_Implement(const std::string& name, int32_t& value, const nxs::Array<std::string>& enumStrings) { return false; }
    bool VisitBitfield_Implement(const std::string&, uint32_t&, const nxs::Array<nxs::BitfieldOption>&) override { return false; }
};