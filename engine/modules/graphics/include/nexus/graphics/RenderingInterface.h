//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/ApplicationContext.h"
#include "nexus/core/Color.h"

#include "GraphicsConfig.h"
#include "GraphicsConst.h"
#include "IndexBuffer.h"
#include "GpuProgram.h"
#include "TextureProxy.h"
#include "VertexBuffer.h"
#include "PipelineState.h"

NXS_NAMESPACE
{
    /**
     * @brief A wrapper interface for the underlying graphics API.
     * This class provides a unified interface for rendering operations,
     * allowing the engine to support multiple graphics APIs (e.g., OpenGL, Vulkan, Direct3D)
     * without changing the higher-level rendering code.
     */
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

        //! Apply pipeline state with minimal redundant state transitions.
        void ApplyPipelineState(const PipelineState& state);
        const PipelineState& GetPipelineState() const { return m_pipelineState; }

        void SetClearColor(const Color4F& color) { m_pipelineState.clearColor = color; }
        void SetClearColor(uint32_t rgba) { m_pipelineState.clearColor = rgba; }
        NODISCARD const Color4F& GetClearColor() const { return m_pipelineState.clearColor; }

        void SetClearDepth(float depth) { m_pipelineState.clearDepth = depth; }
        NODISCARD float GetClearDepth() const { return m_pipelineState.clearDepth; }

        virtual void ClearColor(const Color4F& color)
        {
            NXS_ASSERT_MSG(false, "ClearColor not implemented for this graphics API.");
        }

        virtual void ClearDepth(float depth)
        {
            NXS_ASSERT_MSG(false, "ClearDepth not implemented for this graphics API.");
        }

        virtual void ClearBuffer(const Color4F& color, float depth)
        {
            NXS_ASSERT_MSG(false, "ClearBuffer not implemented for this graphics API.");
        }

        virtual void SwapBuffer()
        {
            NXS_ASSERT_MSG(false, "SwapBuffer not implemented for this graphics API.");
        }

        virtual void SetViewport(int32_t x, int32_t y, int32_t w, int32_t h)
        {
            NXS_ASSERT_MSG(false, "SetViewport not implemented for this graphics API.");
        }

        NODISCARD virtual VertexBuffer* CreateVertexBuffer() const
        {
            NXS_ASSERT_MSG(false, "CreateVertexBuffer not implemented for this graphics API.");
            return nullptr;
        }

        NODISCARD virtual IndexBuffer* CreateIndexBuffer() const
        {
            NXS_ASSERT_MSG(false, "CreateIndexBuffer not implemented for this graphics API.");
            return nullptr;
        }

        NODISCARD virtual GpuProgram* CreateGpuProgram() const
        {
            NXS_ASSERT_MSG(false, "CreateGpuProgram not implemented for this graphics API.");
            return nullptr;
        }

        NODISCARD virtual TextureProxy* CreateTexture() const
        {
            NXS_ASSERT_MSG(false, "CreateTexture not implemented for this graphics API.");
            return nullptr;
        }

        virtual void OnResize(uint32_t pixel_w, uint32_t pixel_h)
        {
            NXS_ASSERT_MSG(false, "OnResize not implemented for this graphics API.");
        }

        virtual void DrawIndexed(const Ref<IndexBuffer> indexBuffer)
        {
            NXS_ASSERT_MSG(false, "DrawIndexed not implemented for this graphics API.");
        }

        virtual void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32_t instanceCount)
        {
            NXS_ASSERT_MSG(false, "DrawIndexedInstanced not implemented for this graphics API.");
        }

        NODISCARD GraphicsAPI GetAPI() const
        {
            return m_graphicsAPI;
        }

        virtual void SetColorMask(const glm::bvec4& mask) { m_pipelineState.colorMask = mask; }
        NODISCARD const glm::bvec4& GetColorMask() const { return m_pipelineState.colorMask; }

        virtual void SetDepthWrite(bool write) { m_pipelineState.depthWrite = write; }
        NODISCARD bool GetDepthWrite() const { return m_pipelineState.depthWrite; }
        NODISCARD bool GetDepthTest() const { return m_pipelineState.depthTest; }

        virtual void SetDepthFunction(DepthFunction depthFunction);
        NODISCARD DepthFunction GetDepthFunction() const { return m_pipelineState.depthFunction; }

        virtual void SetBlendMode(BlendMode blendMode) { m_pipelineState.overrideBlendMode = blendMode; }
        NODISCARD BlendMode GetBlendMode() const { return m_pipelineState.overrideBlendMode; }

        virtual void SetLineWidth(float width) { m_pipelineState.lineWidth = width; }
        NODISCARD float GetLineWidth() const { return m_pipelineState.lineWidth; }

        virtual void SetStencilTest(bool enabled) { m_pipelineState.stencilTest = enabled; }
        NODISCARD bool GetStencilTest() const { return m_pipelineState.stencilTest; }

        virtual void SetStencilOperation(StencilOperation operation) { m_pipelineState.stencilOp = operation; }
        NODISCARD StencilOperation GetStencilOperation() const { return m_pipelineState.stencilOp; }

        virtual void SetPolygonMode(PolygonMode mode) { m_pipelineState.polygonMode = mode; }
        NODISCARD virtual PolygonMode GetPolygonMode() const { return m_pipelineState.polygonMode; }

        virtual void SetCullMode(PolygonFacing face) { m_pipelineState.cullMode = face; }
        NODISCARD PolygonFacing GetCullMode() const { return m_pipelineState.cullMode; }

        virtual void SetFrontFace(FrontFace face);
        NODISCARD FrontFace GetFrontFace() const { return m_pipelineState.frontFace; }

        NODISCARD Ref<GpuProgram> GetGlobalShader() const { return m_pipelineState.globalShader; }
        void SetGlobalShader(Ref<GpuProgram> shader) { m_pipelineState.globalShader = std::move(shader); }

        virtual void EnableDrawBuffer(DrawBuffer buffer)
        {
            NXS_ASSERT_MSG(false, "EnableDrawBuffer not implemented for this graphics API.");
        }

        virtual void DebugMarker(const std::string& message)
        {
            NXS_ASSERT_MSG(false, "DebugMarker not implemented for this graphics API.");
        }

    protected:
        RenderingInterface() = default;
        virtual ~RenderingInterface();

    protected:
        RenderContext m_renderContext {};
        GraphicsAPI m_graphicsAPI = GraphicsAPI::Undefined;
        PipelineState m_pipelineState {
            .depthFunction = DepthFunction::None,
        };
    };
}
