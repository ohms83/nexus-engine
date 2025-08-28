//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include "../VertexBuffer.h"

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
        bool IsBinding() const override;

        void CopyData(const void* data, size_t bytes, size_t offset = 0) override;

    private:
        //! API specific vertex buffer generation function.
        void Build_Impl() override;

        static uint32 s_bindingBuffer;
        //! For thread safety.
        static std::mutex s_mutex;

    protected:
        NODISCARD uint32 Alloc() override;
        void Release() override;

    private:
        //! Vertex buffer handle
        GLuint m_vbo = 0;
    };
}
