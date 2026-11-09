//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include "nexus/container/Array.h"
#include "../VertexBuffer.h"

NXS_NAMESPACE
{
    class GLVertexBuffer final : public VertexBuffer
    {
    public:
        GLVertexBuffer() = default;
        ~GLVertexBuffer() override;

        VertexBuffer& Begin() override;

        VertexBuffer& AttachInstanceStream(Ref<IBuffer> instanceData, const Array<VertexAttribute>& attributes, BufferUsage usage = BufferUsage::StaticDraw) override;
        void DetachInstanceStreams() override;
        void Bind() const override;
        void Unbind() const override;
        bool IsBinding() const override;

        void CopyData(const void* data, size_t bytes, size_t offset = 0) override;

    private:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;

    protected:
        NODISCARD uint32_t Alloc() override;
        void Release() override;

    private:
        //! Vertex buffer handle
        GLuint m_vbo = 0;
        // For instance streams, store GL VBO handles so we can free them later
        Array<uint32_t> m_instanceVBOs;
        // We keep borrowed instance buffers to delete when detached
        Array<VertexInstanceStream> m_instanceStreamsLocal;
    };
}
