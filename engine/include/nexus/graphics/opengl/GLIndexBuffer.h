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
        GLIndexBuffer() = default;
        ~GLIndexBuffer() override;

        void Bind() override;
        void Unbind() override;

    private:
        uint32 GenBuffer() override;
        void Build_Impl() override;
    };
}
