//
// Created by nutta on 7/9/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/GraphicsConst.h"

#include "Menu.h"

#include "widget/EditorWidget.h"

#include <vector>

NXS_NAMESPACE
{
    // TODO: Refactoring
    class Application;
    class RenderSystem;

    class Editor final : public IWidgetOwner
    {
    public:
        Editor(Application& parentApp);
        ~Editor();

        Application& GetParentApp() const { return m_parentApp; }
        WindowContext GetWindowContext() const override;

        void Update();
        void Draw(RenderSystem& renderSystem);

        Menu& GetMenu() const { return *m_menu.get(); };

        void AddWidget(Ref<EditorWidget> widget) override
        {
            m_widgets.push_back(widget);
        }

        void Clear();

    protected:
        void InitMenu();
        void DrawMainMenu();

        void UpdateWidgets() override;
        void DrawWidgets(RenderSystem& renderSystem) const override;

    protected:
        Application& m_parentApp;
        WindowContext m_windowContext {};
        Ptr<Menu> m_menu;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}