//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <vector>
#include <nexus/NxsDefine.h>

#include "nexus/graphics/GraphicsConst.h"
#include "Console.h"
#include "Profiler.h"

NXS_NAMESPACE
{
    class RenderSystem;

    struct EditorConfig
    {
        GraphicsAPI renderingBackend;
    };

    struct MenuItem
    {
        std::string name;
        std::string category;
        std::string description;
        std::string shortcut;
        Ref<EditorWidget> widget;
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
        void DrawProfiler(const RenderSystem& renderSystem);
        void DrawMainMenu(const RenderSystem& renderSystem);

    protected:
        EditorConfig m_config{};
        //! For the profiler
        std::vector<uint32> m_frameTimes;
        std::vector<uint32> m_frameCounters;

        Ptr<Console> m_console;

        struct MenuItemList
        {
            std::string menu;
            std::vector<MenuItem> items;
        };

        std::vector<MenuItemList> m_menuItems;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}