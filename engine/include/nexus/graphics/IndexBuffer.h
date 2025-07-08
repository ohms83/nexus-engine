//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <vector>

#include "GPUBuffer.h"
#include "GpuResrouce.h"
#include "GraphicsConst.h"

NXS_NAMESPACE
{
    class IndexBuffer : public GpuResrouce
    {
    public:
        IndexBuffer() = default;

        virtual IndexBuffer& Begin();
        virtual IndexBuffer& SetIndices(uint32* indices, size_t num);
        virtual IndexBuffer& SetIndices(std::vector<uint32>&& indices);
        virtual IndexBuffer& SetUsage(BufferUsage usage);
        virtual IndexBuffer& SetDrawMode(DrawMode mode);
        virtual void Build();

        [[nodiscard]] uint32 NumIndex() const { return m_indices.size(); }
        [[nodiscard]] uint32 NumPolygons() const;

        [[nodiscard]] BufferUsage GetUsage() const
        {
            return m_usage;
        }

        [[nodiscard]] DrawMode GetDrawMode() const
        {
            return m_drawMode;
        }

    private:
        //! Finalizing the buffer generation based on all the provided data.
        virtual void Build_Impl() = 0;

    protected:
        bool m_hasBuilt = false;
        BufferUsage m_usage;
        DrawMode m_drawMode = DrawMode::Triangle;
        std::vector<uint32> m_indices;
    };
}