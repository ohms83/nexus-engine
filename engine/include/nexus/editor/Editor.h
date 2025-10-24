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
    class RenderSystem;

    class Editor final : public IWidgetOwner
    {
    public:
        Editor();
        ~Editor();

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
        Ptr<Menu> m_menu;
        std::vector<Ref<EditorWidget>> m_widgets;
    };
}