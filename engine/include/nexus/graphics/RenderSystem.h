#pragma once

#include <nexus/NxsDefine.h>
#include "Color.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    class VertexBuffer;
    class IndexBuffer;
    class Shader;

    class RenderSystem final
    {
    public:
        explicit RenderSystem(GraphicsConfig config);
        ~RenderSystem();

        void ClearScreen() const;

        void SetClearColor(const Color4B& color = COLOR4B_GREY)
        {
            m_clearColor = color;
        }

        void SetClearColor(const uint32_t rgba)
        {
            m_clearColor = rgba;
        }

        [[nodiscard]] const Color4B& GetClearColor() const
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
        void EndDraw();

        //! An event handler called when the window resize event occured.
        void OnResize(uint32_t width, uint32_t height);

    protected:
        GraphicsConfig m_config;
        RenderingInterface* m_renderingInterface = nullptr;
        Color4B m_clearColor = COLOR4B_GREY;
        float m_clearDepth = 1.0f;
    };
}