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

    protected:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;

    private:
        GLuint m_vbo, m_vao;
    };
}
