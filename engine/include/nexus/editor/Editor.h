//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "nexus/graphics/GraphicsConst.h"

NXS_NAMESPACE
{
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
        virtual void Draw(const RenderSystem& renderSystem) const;
        void EndDraw() const;

    protected:
        EditorConfig m_config{};
    };
}