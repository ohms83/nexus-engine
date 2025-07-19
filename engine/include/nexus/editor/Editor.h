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

    struct EditorConfig
    {
        GraphicsAPI renderingBackend;
    };

    class Editor
    {
    public:
        Editor() = delete;
        Editor(WindowContext window, RenderContext renderContext, const EditorConfig& config);
        virtual ~Editor();

        void Update(const SDL_Event& event);

        void BeginDraw() const;
        virtual void Draw(const RenderSystem& renderSystem);
        void EndDraw() const;

        void AddMenuItem(const std::string& menu, const MenuItem& menuItem);

    protected:
        void InitMenu();
        void DrawMainMenu(const RenderSystem& renderSystem);

    protected:
        EditorConfig m_config{};

        struct MenuItemList
        {
            std::string menu;
            std::vector<MenuItem> items;
        };

        std::vector<MenuItemList> m_menuItems;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}