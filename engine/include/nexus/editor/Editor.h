//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <vector>
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
        virtual void Draw(const RenderSystem& renderSystem);
        void EndDraw() const;

    protected:
        void DrawProfiler(const RenderSystem& renderSystem);

    protected:
        EditorConfig m_config{};
        //! For the profiler
        std::vector<uint32> m_frameTimes;
        std::vector<uint32> m_frameCounters;
    };
}