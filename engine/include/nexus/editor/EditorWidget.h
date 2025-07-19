//
// Created by nutta on 7/16/2025.
//

#pragma once

#include <string>
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

        explicit EditorWidget(const std::string& name): m_name(name) {}
        virtual ~EditorWidget() = default;

        //! Render this widget.
        void Draw(const RenderSystem& renderSystem);
        virtual void Update() {}

        Visibility GetVisibility() const { return m_visibility; }
        void Show();
        void Hide();

    protected:
        virtual void BeginDraw(const RenderSystem& renderSystem);
        virtual void Draw_Internal(const RenderSystem& renderSystem) = 0;
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
