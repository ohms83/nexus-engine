//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <vector>

#include "GPUBuffer.h"

NXS_NAMESPACE
{
    class IndexBuffer
    {
    public:
        IndexBuffer(std::vector<uint32>&& indices, BufferUsage usage)
            : m_indices(indices)
        {
        }

        virtual ~IndexBuffer() = default;

        virtual void Bind() = 0;

        uint32 NumIndex() const { return m_indices.size(); }

    protected:
        std::vector<uint32> m_indices;
    };
}