//
// Created by nutta on 9/17/2025.
//
#include "editor/DebugMenuItem.h"

#include "Engine.h"

USING_NAMESPACE_NXS;

#define DEBUG_DEFAULT_GROUP 0

// static const MenuItem s_renderModeWireframe = {
//     // Group
//     DEBUG_DEFAULT_GROUP,
//     MenuType::Toggle,
//     // Name
//     "Render mode Wireframe",
//     // Descriptions
//     "Render all the object in wireframe.",
//     // Shortcut
//     "",
//     // Widget
//     nullptr,
//     // Menu handler
//     [](const MenuItem&)
//     {
//         const auto renderingInterface = Engine::Instance().GetRenderingInterface();
//         renderingInterface->SetCullMode(PolygonFacing::None);
//         renderingInterface->SetPolygonMode(PolygonMode::Wireframe);
//     }
// };

// static const MenuItem s_renderModeFill = {
//     // Group
//     DEBUG_DEFAULT_GROUP,
//     MenuType::Toggle,
//     // Name
//     "Render mode Fill",
//     // Descriptions
//     "",
//     // Shortcut
//     "",
//     // Widget
//     nullptr,
//     // Menu handler
//     [](const MenuItem&)
//     {
//         const auto renderingInterface = Engine::Instance().GetRenderingInterface();
//         renderingInterface->SetCullMode(PolygonFacing::Back);
//         renderingInterface->SetPolygonMode(PolygonMode::Fill);
//     }
// };

// static const MenuItem s_renderModeTwoSide = {
//     // Group
//     DEBUG_DEFAULT_GROUP,
//     MenuType::Toggle,
//     // Name
//     "Render mode Two-Side",
//     // Descriptions
//     "Disable backface culling.",
//     // Shortcut
//     "",
//     // Widget
//     nullptr,
//     // Menu handler
//     [](const MenuItem&)
//     {
//         const auto renderingInterface = Engine::Instance().GetRenderingInterface();
//         renderingInterface->SetCullMode(PolygonFacing::None);
//     }
// };

// NXS_NAMESPACE
// {
//     static const std::vector<MenuItem> s_debugMenuItems = {
//         s_renderModeWireframe,
//         s_renderModeFill,
//         s_renderModeTwoSide,
//     };
// }