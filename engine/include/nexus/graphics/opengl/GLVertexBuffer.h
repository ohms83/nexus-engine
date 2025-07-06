//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/VertexBuffer.h>

NXS_NAMESPACE
{
    class GLVertexBuffer : public VertexBuffer
    {
    public:
        GLVertexBuffer();
        ~GLVertexBuffer() override;

        VertexBuffer& Begin(Buffer&& vertexData, BufferUsage usage) override;
        VertexBuffer& Begin(uint8* data, size_t size, BufferUsage usage) override;

    protected:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;

    private:
        GLuint m_vao = 0;
        //! Vertex buffer handle
        GLuint m_vbo = 0;
        //! Index buffer handle
        GLuint m_ebo = 0;
    };
}
