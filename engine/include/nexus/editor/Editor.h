//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include <vector>

#include "MenuItem.h"
#include "nexus/graphics/GraphicsConst.h"

NXS_NAMESPACE
{
    class EditorWidget;
    class RenderSystem;

    class Editor final
    {
    public:
        Editor();

        void Update() const;
        void Draw(RenderSystem& renderSystem);

        void AddMenuItem(const std::string& menu, const MenuItem& menuItem);

    protected:
        void InitMenu();
        void DrawMainMenu();

    protected:
        struct MenuItemList
        {
            std::string menu;
            std::vector<MenuItem> items;
        };

        std::vector<MenuItemList> m_menuItems;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}