#pragma once

#include <nexus/NxsDefine.h>
#include "Color.h"

NXS_NAMESPACE
{
    enum class GraphicsAPI
    {
        Undefined,
        OpenGL,
        OpenGLES,
        Vulkan,
        D3D11,
        D3D12,
    };

    class RenderSystem
    {
    public:
        RenderSystem(GraphicsAPI api);

        void ClearScreen();

        void SetClearColor(const Color4F& color = COLOR4F_GREY)
        {
            m_clearColor = color;
        }

        const Color4F& GetClearColor() const
        {
            return m_clearColor;
        }
        
        void BeginDraw();
        void Draw();
        void EndDraw();

    protected:
        GraphicsAPI m_apiType = GraphicsAPI::Undefined;
        Color4F m_clearColor = COLOR4F_GREY;
    };
}