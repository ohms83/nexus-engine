//
// Created by nutta on 9/17/2025.
//
#include "editor/DebugMenuItem.h"

#include "Engine.h"

USING_NAMESPACE_NXS;

#define DEBUG_DEFAULT_GROUP 0

static const MenuItem RenderModeWireframe = {
    // Group
    DEBUG_DEFAULT_GROUP,
    // Name
    "Render mode Wireframe",
    // Descriptions
    "",
    // Shortcut
    "",
    // Widget
    nullptr,
    // Menu handler
    [](const MenuItem&)
    {
        const auto renderingInterface = Engine::Instance().GetRenderingInterface();
        renderingInterface->SetCullMode(PolygonFacing::None);
        renderingInterface->SetPolygonMode(PolygonMode::Wireframe);
    }
};

static const MenuItem RenderModeFill = {
    // Group
    DEBUG_DEFAULT_GROUP,
    // Name
    "Render mode Fill",
    // Descriptions
    "",
    // Shortcut
    "",
    // Widget
    nullptr,
    // Menu handler
    [](const MenuItem&)
    {
        const auto renderingInterface = Engine::Instance().GetRenderingInterface();
        renderingInterface->SetCullMode(PolygonFacing::Back);
        renderingInterface->SetPolygonMode(PolygonMode::Fill);
    }
};

NXS_NAMESPACE
{
    static const std::vector<MenuItem> s_debugMenuItems = {
        RenderModeWireframe,
        RenderModeFill,
    };
}