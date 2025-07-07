//
// Created by nutta on 7/4/2025.
//

#pragma once

#include <cassert>
#include <string>
#include <nexus/NxsDefine.h>

#include "Color.h"
#include "GraphicsConfig.h"
#include "IndexBuffer.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexBuffer.h"

NXS_NAMESPACE
{
    class RenderingInterface
    {
    public:
        RenderingInterface(const RenderingInterface&) = delete;
        RenderingInterface(RenderingInterface&&) = delete;

        static RenderingInterface* Create(WindowContext window, const GraphicsConfig& config);
        static void Destroy();

        static RenderingInterface& Get()
        {
            assert(m_singleton);
            // ReSharper disable once CppDFANullDereference
            return *m_singleton;
        }

        virtual void ClearColor(const Color4F& color) = 0;
        virtual void ClearDepth(float depth) = 0;
        virtual void ClearBuffer(const Color4F& color, float depth) = 0;

        virtual void SwapBuffer() = 0;

        virtual void SetViewport(int32 x, int32 y, int32 w, int32 h) = 0;

        [[nodiscard]] virtual VertexBuffer* CreateVertexBuffer() const = 0;
        [[nodiscard]] virtual IndexBuffer* CreateIndexBuffer() const = 0;
        [[nodiscard]] virtual Shader* CreateShader() const = 0;

        virtual void OnResize(uint32_t pixel_w, uint32_t pixel_h) = 0;

        void Draw(const RenderCommand& command);

    protected:
        RenderingInterface() = default;
        virtual ~RenderingInterface() = default;

        virtual void Draw_Internal(const RenderCommand& command) = 0;

    private:
        static RenderingInterface* m_singleton;
    };
}
