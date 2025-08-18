//
// Created by nutta on 8/18/2025.
//
#include <random>

#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

TEST(DirectionalLightTest, GetterSetter)
{
    entt::registry registry;
    DirectionalLight light(registry);

    light.SetDiffuseColor(Color3F::Red);
    EXPECT_TRUE(Color3F::Red == light.GetDiffuseColor());

    light.SetEmissiveColor(Color3F::Blue);
    EXPECT_TRUE(Color3F::Blue == light.GetEmissiveColor());

    light.SetSpecularColor(Color3F::Green);
    EXPECT_TRUE(Color3F::Green == light.GetSpecularColor());

    EXPECT_FLOAT_EQ(light.GetCutoffRange(), NXS_INFINITE);

    const auto name = "Directional Light";
    light.SetName(name);
    EXPECT_TRUE(light.GetName() == name);
}

TEST(PointLightTest, GetterSetter)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0);

    entt::registry registry;
    PointLight light(registry);

    light.SetDiffuseColor(Color3F::Red);
    EXPECT_TRUE(Color3F::Red == light.GetDiffuseColor());

    light.SetEmissiveColor(Color3F::Blue);
    EXPECT_TRUE(Color3F::Blue == light.GetEmissiveColor());

    light.SetSpecularColor(Color3F::Green);
    EXPECT_TRUE(Color3F::Green == light.GetSpecularColor());

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