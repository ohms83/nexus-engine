#include "gtest/gtest.h"
#include "nexus/reflection/Reflection.h"

#include <string>
#include <vector>

namespace
{
    class RecordingReflector final : public nxs::IReflector
    {
    public:
        void SetMarker(const std::string&) override {}
        void SetReadOnlyFlag(bool) override {}

        bool changed = true;
        std::vector<std::string> visits;

    private:
        bool Record(const std::string& type)
        {
            visits.push_back(type);
            return changed;
        }

        bool VisitBool_Implement(const std::string&, bool&) override { return Record("Bool"); }
        bool VisitInt_Implement(const std::string&, int32_t&) override { return Record("Int"); }
        bool VisitUInt_Implement(const std::string&, uint32_t&) override { return Record("UInt"); }
        bool VisitInt64_Implement(const std::string&, int64_t&) override { return Record("Int64"); }
        bool VisitUInt64_Implement(const std::string&, uint64_t&) override { return Record("UInt64"); }
        bool VisitFloat_Implement(const std::string&, float&) override { return Record("Float"); }
        bool VisitDouble_Implement(const std::string&, double&) override { return Record("Double"); }
        bool VisitString_Implement(const std::string&, std::string&) override { return Record("String"); }
        bool VisitVec2_Implement(const std::string&, glm::vec2&) override { return Record("Vec2"); }
        bool VisitVec3_Implement(const std::string&, glm::vec3&) override { return Record("Vec3"); }
        bool VisitVec4_Implement(const std::string&, glm::vec4&) override { return Record("Vec4"); }
        bool VisitColor3_Implement(const std::string&, nxs::Color3F&) override { return Record("Color3"); }
        bool VisitColor4_Implement(const std::string&, nxs::Color4F&) override { return Record("Color4"); }
        bool VisitObject_Implement(const std::string&, nxs::IReflection&) override { return Record("Object"); }
        bool VisitEnum_Implement(const std::string&, int32_t&, const nxs::Array<std::string>&) override
        {
            return Record("Enum");
        }
    };

    class ReflectableFixture final : public nxs::IReflection
    {
    public:
        IMPLEMENT_REFLECTION_STRUCT(ReflectableFixture);

        nxs::VariantData Serialize() const override
        {
            return nxs::VariantData(nxs::VariantData::Map{});
        }

        bool Deserialize(const nxs::VariantData&) override { return true; }
        void AcceptReflector(nxs::IReflector&) override {}
    };
}

TEST(IReflectorTest, TypedVisitMethodsDispatchAndNotifyChanges)
{
    RecordingReflector reflector;
    std::vector<std::string> changedProperties;
    auto connection = reflector.onValueChangedEvent.connect([&changedProperties](const std::string& name) {
        changedProperties.push_back(name);
    });

    bool boolValue = false;
    int32_t intValue = 0;
    uint32_t uintValue = 0;
    int64_t int64Value = 0;
    uint64_t uint64Value = 0;
    float floatValue = 0.0f;
    double doubleValue = 0.0;
    std::string stringValue;
    glm::vec2 vec2Value{};
    glm::vec3 vec3Value{};
    glm::vec4 vec4Value{};
    nxs::Color3F color3Value{};
    nxs::Color4F color4Value{};
    ReflectableFixture object;
    int32_t enumValue = 0;
    nxs::Array<std::string> enumStrings{"First", "Second"};

    EXPECT_TRUE(reflector.VisitBool("bool", boolValue));
    EXPECT_TRUE(reflector.VisitInt("int", intValue));
    EXPECT_TRUE(reflector.VisitUInt("uint", uintValue));
    EXPECT_TRUE(reflector.VisitInt64("int64", int64Value));
    EXPECT_TRUE(reflector.VisitUInt64("uint64", uint64Value));
    EXPECT_TRUE(reflector.VisitFloat("float", floatValue));
    EXPECT_TRUE(reflector.VisitDouble("double", doubleValue));
    EXPECT_TRUE(reflector.VisitString("string", stringValue));
    EXPECT_TRUE(reflector.VisitVec2("vec2", vec2Value));
    EXPECT_TRUE(reflector.VisitVec3("vec3", vec3Value));
    EXPECT_TRUE(reflector.VisitVec4("vec4", vec4Value));
    EXPECT_TRUE(reflector.VisitColor3("color3", color3Value));
    EXPECT_TRUE(reflector.VisitColor4("color4", color4Value));
    EXPECT_TRUE(reflector.VisitObject("object", object));
    EXPECT_TRUE(reflector.VisitEnum("enum", enumValue, enumStrings));

    const std::vector<std::string> expectedTypes{
        "Bool", "Int", "UInt", "Int64", "UInt64", "Float", "Double", "String",
        "Vec2", "Vec3", "Vec4", "Color3", "Color4", "Object", "Enum"
    };
    const std::vector<std::string> expectedProperties{
        "bool", "int", "uint", "int64", "uint64", "float", "double", "string",
        "vec2", "vec3", "vec4", "color3", "color4", "object", "enum"
    };
    EXPECT_EQ(reflector.visits, expectedTypes);
    EXPECT_EQ(changedProperties, expectedProperties);

    reflector.onValueChangedEvent.disconnect(connection);
}

TEST(IReflectorTest, VisitTemplateDispatchesSupportedTypes)
{
    RecordingReflector reflector;
    std::vector<std::string> changedProperties;
    auto connection = reflector.onValueChangedEvent.connect([&changedProperties](const std::string& name) {
        changedProperties.push_back(name);
    });

    bool boolValue = false;
    int32_t intValue = 0;
    uint32_t uintValue = 0;
    int64_t int64Value = 0;
    uint64_t uint64Value = 0;
    float floatValue = 0.0f;
    double doubleValue = 0.0;
    std::string stringValue;
    glm::vec2 vec2Value{};
    glm::vec3 vec3Value{};
    glm::vec4 vec4Value{};
    nxs::Color3F color3Value{};
    nxs::Color4F color4Value{};
    ReflectableFixture object;

    EXPECT_TRUE(reflector.Visit("bool", boolValue));
    EXPECT_TRUE(reflector.Visit("int", intValue));
    EXPECT_TRUE(reflector.Visit("uint", uintValue));
    EXPECT_TRUE(reflector.Visit("int64", int64Value));
    EXPECT_TRUE(reflector.Visit("uint64", uint64Value));
    EXPECT_TRUE(reflector.Visit("float", floatValue));
    EXPECT_TRUE(reflector.Visit("double", doubleValue));
    EXPECT_TRUE(reflector.Visit("string", stringValue));
    EXPECT_TRUE(reflector.Visit("vec2", vec2Value));
    EXPECT_TRUE(reflector.Visit("vec3", vec3Value));
    EXPECT_TRUE(reflector.Visit("vec4", vec4Value));
    EXPECT_TRUE(reflector.Visit("color3", color3Value));
    EXPECT_TRUE(reflector.Visit("color4", color4Value));
    EXPECT_TRUE(reflector.Visit("object", object));

    const std::vector<std::string> expectedTypes{
        "Bool", "Int", "UInt", "Int64", "UInt64", "Float", "Double", "String",
        "Vec2", "Vec3", "Vec4", "Color3", "Color4", "Object"
    };
    const std::vector<std::string> expectedProperties{
        "bool", "int", "uint", "int64", "uint64", "float", "double", "string",
        "vec2", "vec3", "vec4", "color3", "color4", "object"
    };
    EXPECT_EQ(reflector.visits, expectedTypes);
    EXPECT_EQ(changedProperties, expectedProperties);

    reflector.onValueChangedEvent.disconnect(connection);
}

TEST(IReflectorTest, DoesNotNotifyWhenAVisitReportsNoChange)
{
    RecordingReflector reflector;
    int changeNotifications = 0;
    auto connection = reflector.onValueChangedEvent.connect([&changeNotifications](const std::string&) {
        ++changeNotifications;
    });
    reflector.changed = false;

    int32_t intValue = 0;
    float floatValue = 0.0f;
    EXPECT_FALSE(reflector.VisitInt("int", intValue));
    EXPECT_FALSE(reflector.Visit("float", floatValue));
    EXPECT_EQ(changeNotifications, 0);
    EXPECT_EQ(reflector.visits, (std::vector<std::string>{"Int", "Float"}));

    reflector.onValueChangedEvent.disconnect(connection);
}
