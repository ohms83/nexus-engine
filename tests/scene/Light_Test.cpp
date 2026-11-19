//
// Created by nutta on 8/18/2025.
//
#include <random>

#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/FakeReflector.h"

USING_NAMESPACE_NXS;

namespace
{
    class LightTestReflector final : public FakeReflector
    {
    public:
        bool sawShadowFlag = false;
    private:
        bool VisitBool_Implement(const std::string& name, bool&) override
        {
            if (name == "Can Cast Shadows") sawShadowFlag = true;
            return false;
        }
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