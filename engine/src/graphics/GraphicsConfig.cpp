//
// Created by nutta on 8/5/2025.
//
#include "nexus/graphics/GraphicsConfig.h"
#include "nexus/core/serialize/IniParser.h"

USING_NAMESPACE_NXS;

bool GraphicsConfig::LoadConfig(const std::string& filePath)
{
    IniParser parser;
    if (!parser.Load(filePath)) return false;

    constexpr auto sectionName = "graphics";

    if (const auto apiName = parser.GetValue(sectionName, "api"); apiName.has_value())
    {
        const std::map<std::string, GraphicsAPI> apiNameMap = {
            { "Undefined", GraphicsAPI::Undefined },
            { "OpenGL", GraphicsAPI::OpenGL },
            { "OpenGLES", GraphicsAPI::OpenGLES },
            { "Vulkan", GraphicsAPI::Vulkan },
            { "D3D11", GraphicsAPI::D3D11 },
            { "D3D12", GraphicsAPI::D3D12 },
        };
        if (const auto& itr = apiNameMap.find(*apiName); itr != apiNameMap.end()) api = itr->second;
    }

    if (const auto value = parser.GetValue<int>(sectionName, "screenWidth"); value.has_value()) screenWidth = *value;
    if (const auto value = parser.GetValue<int>(sectionName, "screenHeight"); value.has_value()) screenHeight = *value;
    if (const auto value = parser.GetBoolValue(sectionName, "vsync"); value.has_value()) vsync = *value;

    return true;
}
