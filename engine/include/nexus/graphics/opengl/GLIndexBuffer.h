//
// Created by nutta on 7/6/2025.
//

#pragma once

#include "NxsGL.h"
#include "../IndexBuffer.h"

#include <atomic>

NXS_NAMESPACE
{
    class GLIndexBuffer final : public IndexBuffer
    {
    public:
        GLIndexBuffer() = default;
        ~GLIndexBuffer() override;

        void Bind() const override;
        void Unbind() const override;
        bool IsBinding() const override;

        void CopyData(const void* data, size_t bytes, size_t offset = 0) override;

    private:
        void Build_Impl() override;

        static std::atomic<uint32_t> s_bindingBuffer;

    protected:
        uint32_t Alloc() override;
        void Release() override;
    };
}
