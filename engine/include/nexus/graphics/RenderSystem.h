#pragma once

#include <nexus/NxsDefine.h>
#include "Color.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    class RenderSystem final
    {
    public:
        explicit RenderSystem(WindowContext window, GraphicsConfig config);
        ~RenderSystem();

        void ClearScreen() const;

        void SetClearColor(const Color4F& color)
        {
            m_clearColor = color;
        }

        void SetClearColor(const uint32_t rgba)
        {
            m_clearColor = rgba;
        }

        [[nodiscard]] const Color4F& GetClearColor() const
        {
            return m_clearColor;
        }

        void SetClearDepth(const float depth)
        {
            m_clearDepth = depth;
        }

        [[nodiscard]] float GetClearDepth() const
        {
            return m_clearDepth;
        }
        
        void BeginDraw() const;
        void Draw();
        void EndDraw() const;

        //! An event handler called when the window resize event occured.
        void OnResize(uint32_t pixel_w, uint32_t pixel_h) const;

    protected:
        GraphicsConfig m_config;
        RenderingInterface* m_renderingInterface = nullptr;
        Color4F m_clearColor = COLOR4F_GREY;
        float m_clearDepth = 1.0f;
    };
}