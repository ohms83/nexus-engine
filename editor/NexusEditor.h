//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <nexus/Nexus.h>

class NexusEditor final : public nxs::Application
{
public:
    ~NexusEditor() override;

protected:
    bool Init_Internal() override;
    void OnEvent(const SDL_Event& e) override;
    void Render(nxs::RenderSystem& renderSystem) override;
    void OnKeyDown(SDL_Keycode key) override;
    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override;

    void InitMenu();
    void DrawMainMenu(nxs::RenderSystem& renderSystem);

    void AddMenuItem(const std::string& menu, const nxs::MenuItem& menuItem);

private:

    struct MenuItemList
    {
        std::string menu;
        std::vector<nxs::MenuItem> items;
    };

    std::vector<MenuItemList> m_menuItems;
    std::vector<nxs::Ref<nxs::EditorWidget>> m_widgets;
};