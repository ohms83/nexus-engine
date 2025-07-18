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
        explicit EditorWidget(const std::string& name): m_name(name) {}
        virtual ~EditorWidget() = default;

        //! Render this widget.
        void Draw(const RenderSystem& renderSystem);
        virtual void Update() {}

        bool visible = true;

    protected:
        virtual void BeginDraw(const RenderSystem& renderSystem);
        virtual void Draw_Internal(const RenderSystem& renderSystem) = 0;
        virtual void EndDraw();

    protected:
        std::string m_name;
    };
}
