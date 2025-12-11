//
// DebugMenuItem.inl
// Created by nutta on 9/17/2025.
//

#include "Engine.h"

#define DEBUG_DEFAULT_GROUP 0

NXS_NAMESPACE
{
    static const MenuItemList s_debugMenuItems = {
        "Debug",
        {
            std::make_shared<ToggleMenuItem> (
                DEBUG_DEFAULT_GROUP,
                // Name
                "Render mode Wireframe",
                // Descriptions
                "Render all the objects in wireframe.",
                // Shortcut
                "",
                // Menu handler
                [](bool is_selected)
                {
                    const auto renderingInterface = Engine::Instance().GetRenderingInterface();
                    if (is_selected)
                    {
                        renderingInterface->SetCullMode(PolygonFacing::None);
                        renderingInterface->SetPolygonMode(PolygonMode::Wireframe);
                    }
                    else
                    {
                        renderingInterface->SetCullMode(PolygonFacing::Back);
                        renderingInterface->SetPolygonMode(PolygonMode::Fill);
                    }
                }
            ),

            std::make_shared<ToggleMenuItem> (
                DEBUG_DEFAULT_GROUP,
                // Name
                "Disable Backface Culling",
                // Descriptions
                "Render all the objects' backfaces.",
                // Shortcut
                "",
                // Menu handler
                [](bool is_selected)
                {
                    const auto renderingInterface = Engine::Instance().GetRenderingInterface();
                    if (is_selected)
                    {
                        renderingInterface->SetCullMode(PolygonFacing::None);
                    }
                    else
                    {
                        renderingInterface->SetCullMode(PolygonFacing::Back);
                    }
                }
            ),
        }
    };
}