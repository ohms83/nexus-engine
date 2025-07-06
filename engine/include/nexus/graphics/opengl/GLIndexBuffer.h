//
// Created by nutta on 7/6/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/IndexBuffer.h>

NXS_NAMESPACE
{
    class GLIndexBuffer final : public IndexBuffer
    {
    public:
        GLIndexBuffer(std::vector<uint32>&& indices, BufferUsage usage);
        ~GLIndexBuffer() override;

        void Bind() override;

    private:
        GLuint m_ebo = 0;
    };
}
