//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/VertexBuffer.h>

NXS_NAMESPACE
{
    class GLVertexBuffer final : public VertexBuffer
    {
    public:
        GLVertexBuffer() = default;
        ~GLVertexBuffer() override;

        VertexBuffer& Begin() override;

        void Bind() const override;
        void Unbind() const override;

    protected:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;
        [[nodiscard]] uint32 Alloc() override;

    private:
        //! Vertex buffer handle
        GLuint m_vbo = 0;
    };
}
