//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <vector>

#include "GPUBuffer.h"
#include "GraphicsConst.h"

NXS_NAMESPACE
{
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;
        virtual ~IndexBuffer() = default;

        virtual IndexBuffer& Begin();
        virtual IndexBuffer& SetIndices(uint32* indices, size_t num);
        virtual IndexBuffer& SetIndices(std::vector<uint32>&& indices);
        virtual IndexBuffer& SetUsage(BufferUsage usage);
        virtual IndexBuffer& SetDrawMode(DrawMode mode);
        virtual void Build();

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        [[nodiscard]] uint32 NumIndex() const { return m_indices.size(); }

        [[nodiscard]] uint32 GetHandle() const
        {
            return m_handle;
        }

        [[nodiscard]] BufferUsage GetUsage() const
        {
            return m_usage;
        }

        [[nodiscard]] DrawMode GetDrawMode() const
        {
            return m_drawMode;
        }

    private:
        /**
         * Allocating the buffer object on the GPU. The child classes
         * must provide the implementation of this function.
         * @return The generated index buffer's handle; otherwise 0, if failed.
         */
        virtual uint32 GenBuffer() = 0;
        //! Finalizing the buffer generation based on all the provided data.
        virtual void Build_Impl() = 0;

    protected:
        uint32 m_handle = 0;
        bool m_hasBuilt = false;
        BufferUsage m_usage;
        DrawMode m_drawMode = DrawMode::Triangle;
        std::vector<uint32> m_indices;
    };
}