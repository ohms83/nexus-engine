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

    private:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;
    protected:
        NODISCARD uint32 Alloc() override;
        void Release() override;

    private:
        //! Vertex buffer handle
        GLuint m_vbo = 0;
    };
}
