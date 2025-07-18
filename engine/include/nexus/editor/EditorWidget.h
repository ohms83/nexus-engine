//
// Created by nutta on 7/16/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    class RenderSystem;

    class EditorWidget
    {
    public:
        virtual ~EditorWidget() = default;

        //! Render this widget.
        virtual void Draw(const RenderSystem& renderSystem) = 0;
        virtual void Update() {}

        bool visible = true;
    };
}