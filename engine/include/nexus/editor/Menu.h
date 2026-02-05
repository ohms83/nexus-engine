#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/RenderSystem.h"

NXS_NAMESPACE
{
    class MenuItem;
    class EditorWidget;
    class IWidgetOwner;

    struct MenuItemList
    {
        std::string menu;
        std::vector<Ref<MenuItem>> items;
    };

    class Menu
    {
    public:
        Menu(IWidgetOwner& widgetOwner);

        void Update() const;
        void Draw(RenderSystem& renderSystem);

        void AddMenuItem(const std::string& menu, Ref<MenuItem> menuItem);
        Ref<MenuItem> GetMenuItem(const std::string& menu, const std::string& name);

    private:
        void DrawMenu();

        std::vector<MenuItemList> m_menuItems;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}