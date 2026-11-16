//
// Created by nutta on 8/18/2025.
//
#include <random>

#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

namespace
{
    class LightTestReflector final : public IReflector
    {
    public:
        void SetMarker(const std::string&) override {}
        void SetReadOnlyFlag(bool) override {}
        bool VisitBool(const std::string& name, bool&) override
        {
            if (name == "Can Cast Shadows") sawShadowFlag = true;
            return false;
        }
        bool VisitInt(const std::string&, int32_t&) override { return false; }
        bool VisitUInt(const std::string&, uint32_t&) override { return false; }
        bool VisitInt64(const std::string&, int64_t&) override { return false; }
        bool VisitUInt64(const std::string&, uint64_t&) override { return false; }
        bool VisitFloat(const std::string&, float&) override { return false; }
        bool VisitDouble(const std::string&, double&) override { return false; }
        bool VisitString(const std::string&, std::string&) override { return false; }
        bool VisitVec2(const std::string&, glm::vec2&) override { return false; }
        bool VisitVec3(const std::string&, glm::vec3&) override { return false; }
        bool VisitVec4(const std::string&, glm::vec4&) override { return false; }
        bool VisitColor3(const std::string&, Color3F&) override { return false; }
        bool VisitColor4(const std::string&, Color4F&) override { return false; }
        bool VisitObject(const std::string&, IReflector&) override { return false; }

        bool sawShadowFlag = false;
    };
}

TEST(LightPropertiesTest, ShadowFlagRoundTripsAndLegacyDataDefaultsToEnabled)
{
    LightProperties light;
    EXPECT_TRUE(light.canCastShadows);
    light.canCastShadows = false;
    auto data = light.Serialize();
    LightProperties restored;
    ASSERT_TRUE(restored.Deserialize(data));
    EXPECT_FALSE(restored.canCastShadows);

    data.GetMap().erase("canCastShadows");
    LightProperties legacy;
    ASSERT_TRUE(legacy.Deserialize(data));
    EXPECT_TRUE(legacy.canCastShadows);
}

TEST(LightPropertiesTest, OnlyDirectionalLightExposesShadowFlag)
{
    LightTestReflector reflector;
    DirectLightComponent directional;
    directional.AcceptReflector(reflector);
    EXPECT_TRUE(reflector.sawShadowFlag);

    reflector.sawShadowFlag = false;
    PointLightComponent point;
    point.AcceptReflector(reflector);
    EXPECT_FALSE(reflector.sawShadowFlag);
}

#if 0

TEST(DirectionalLightTest, GetterSetter)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);

    entt::registry registry;
    DirectionalLight light(registry);

    light.SetColor(Color3F::Red);
    EXPECT_TRUE(Color3F::Red == light.GetColor());

    {
        const float value = dis(gen);
        light.SetDiffuseIntensity(value);

        auto equal = glm::epsilonEqual(
            light.GetDiffuseColor(),
            light.GetColor() * value,
            FLT_EPSILON
        );

        EXPECT_TRUE(glm::all(equal));
        EXPECT_FLOAT_EQ(light.GetDiffuseIntensity(), value);
    }

    {
        const float value = dis(gen);
        light.SetSpecularIntensity(value);

        auto equal = glm::epsilonEqual(
            light.GetSpecularColor(),
            light.GetColor() * value,
            FLT_EPSILON
        );

        EXPECT_TRUE(glm::all(equal));
        EXPECT_FLOAT_EQ(light.GetSpecularIntensity(), value);
    }

    EXPECT_FLOAT_EQ(light.GetCutoffRange(), NXS_INFINITE);

    const auto name = "Directional Light";
    light.SetName(name);
    EXPECT_TRUE(light.GetName() == name);
}

TEST(PointLightTest, GetterSetter)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    entt::registry registry;
    PointLight light(registry);

    light.SetColor(Color3F::Red);
    EXPECT_TRUE(Color3F::Red == light.GetColor());

    {
        const float value = dis(gen);
        light.SetDiffuseIntensity(value);

        auto equal = glm::epsilonEqual(
            light.GetDiffuseColor(),
            light.GetColor() * value,
            FLT_EPSILON
        );

        EXPECT_TRUE(glm::all(equal));
        EXPECT_FLOAT_EQ(light.GetDiffuseIntensity(), value);
    }

    {
        const float value = dis(gen);
        light.SetSpecularIntensity(value);

        auto equal = glm::epsilonEqual(
            light.GetSpecularColor(),
            light.GetColor() * value,
            FLT_EPSILON
        );

        EXPECT_TRUE(glm::all(equal));
        EXPECT_FLOAT_EQ(light.GetSpecularIntensity(), value);
    }

    constexpr auto cutoffRange = 500.f;
    light.SetCutoffRange(500.f);
    EXPECT_FLOAT_EQ(cutoffRange, light.GetCutoffRange());

    {
        const auto value = dis(gen);
        light.SetConstantAttenuation(value);
        EXPECT_FLOAT_EQ(value, light.GetConstantAttenuation());
    }

    {
        const auto value = dis(gen);
        light.SetLinearAttenuation(value);
        EXPECT_FLOAT_EQ(value, light.GetLinearAttenuation());
    }

    {
        const auto value = dis(gen);
        light.SetQuadraticAttenuation(value);
        EXPECT_FLOAT_EQ(value, light.GetQuadraticAttenuation());
    }

    const auto name = "Point Light";
    light.SetName(name);
    EXPECT_TRUE(light.GetName() == name);
}

#endif