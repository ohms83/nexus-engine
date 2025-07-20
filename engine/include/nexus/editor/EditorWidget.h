//
// Created by nutta on 7/16/2025.
//

#pragma once

#include <string>
#include <utility>
#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    class RenderSystem;

    class EditorWidget
    {
    public:
        enum class Visibility
        {
            Visible,
            Collapsed,
            //! The widget is closed or hidden.
            Hidden,
        };

        explicit EditorWidget(std::string name): m_name(std::move(name)) {}
        virtual ~EditorWidget() = default;

        //! Render this widget.
        void Draw(RenderSystem& renderSystem);
        virtual void Update() {}

        NODISCARD Visibility GetVisibility() const { return m_visibility; }
        void Show();
        void Hide();

    protected:
        virtual void BeginDraw();
        virtual void Draw_Internal(RenderSystem& renderSystem) = 0;
        virtual void EndDraw();

    protected:
        std::string m_name;
        Visibility m_visibility = Visibility::Visible;

    private:
        /**
         * Internally used for ImGui::Begin. Please don't use this flag for visibility checking
         * but refer to the Visibility status instead.
         */
        bool m_visible = true;
    };
}
