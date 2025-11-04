//
// Created by nutta on 7/4/2025.
//

#pragma once

#include <cassert>
#include "nexus/NxsCommon.h"

#include "Color.h"
#include "GraphicsConfig.h"
#include "IndexBuffer.h"
#include "RenderCommand.h"
#include "GpuProgram.h"
#include "TextureProxy.h"
#include "VertexBuffer.h"

NXS_NAMESPACE
{
    class RenderingInterface
    {
    public:
        RenderingInterface(const RenderingInterface&) = delete;
        RenderingInterface(RenderingInterface&&) = delete;

        static Ref<RenderingInterface> Create(WindowContext window, const GraphicsConfig& config);

        NODISCARD RenderContext GetRenderContext() const
        {
            return m_renderContext;
        }

        virtual void ClearColor(const Color4F& color) = 0;
        virtual void ClearDepth(float depth) = 0;
        virtual void ClearBuffer(const Color4F& color, float depth) = 0;

        virtual void SwapBuffer() = 0;

        virtual void SetViewport(int32 x, int32 y, int32 w, int32 h) = 0;

        NODISCARD virtual VertexBuffer* CreateVertexBuffer() const = 0;
        NODISCARD virtual IndexBuffer* CreateIndexBuffer() const = 0;
        NODISCARD virtual GpuProgram* CreateGpuProgram() const = 0;
        NODISCARD virtual TextureProxy* CreateTexture() const = 0;

        virtual void OnResize(uint32_t pixel_w, uint32_t pixel_h) = 0;

        virtual void DrawIndexed(const Ref<IndexBuffer> indexBuffer) = 0;

        NODISCARD GraphicsAPI GetAPI() const
        {
            return m_graphicsAPI;
        }

        virtual void SetColorMask(const glm::bvec4& mask) = 0;
        virtual void SetDepthMask(bool mask) = 0;

        virtual void SetDepthFunction(DepthFunction depthFunction) = 0;

        virtual void SetLineWidth(float width) = 0;

        virtual void SetPolygonMode(PolygonMode mode) { m_polygonMode = mode; }
        NODISCARD virtual PolygonMode GetPolygonMode() const { return m_polygonMode; }

        virtual void SetCullMode(PolygonFacing face) { m_cullMode = face; }
        NODISCARD PolygonFacing GetCullMode() const { return m_cullMode; }

        virtual void SetFrontFace(FrontFace face) { m_frontFace = face; }
        NODISCARD FrontFace GetFrontFace() const { return m_frontFace; }

        NODISCARD Ref<GpuProgram> GetGlobalShader() const { return m_globalShader; }
        void SetGlobalShader(Ref<GpuProgram> shader) { m_globalShader = shader; }

        virtual void EnableDrawBuffer(DrawBuffer buffer) = 0;

    protected:
        RenderingInterface() = default;
        virtual ~RenderingInterface();

    protected:
        RenderContext m_renderContext{};
        GraphicsAPI m_graphicsAPI = GraphicsAPI::Undefined;
        DepthFunction m_depthFunction = DepthFunction::None;
        PolygonMode m_polygonMode = PolygonMode::Fill;
        PolygonFacing m_cullMode = PolygonFacing::Back;
        FrontFace m_frontFace = FrontFace::CounterClockWise;
        Ref<GpuProgram> m_globalShader;
    };
}
