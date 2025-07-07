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

        void Bind() const override;
        void Unbind() const override;

    private:
        uint32 Alloc() override;
        void Build_Impl() override;
    };
}
