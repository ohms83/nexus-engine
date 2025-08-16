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
    class IndexBuffer : public IGpuResrouce
    {
    public:
        IndexBuffer() = default;

        uint32 GetHandle() const override
        {
            return m_handle;
        }

        virtual IndexBuffer& Begin();
        virtual IndexBuffer& SetIndices(uint32* indices, size_t num, FrontFace frontFace);
        virtual IndexBuffer& SetIndices(std::vector<uint32>&& indices, FrontFace frontFace);
        virtual IndexBuffer& SetUsage(BufferUsage usage);
        virtual IndexBuffer& SetDrawMode(DrawMode mode);
        virtual void Build();

        NODISCARD uint32 NumIndex() const { return m_indices.size(); }
        NODISCARD uint32 NumPolygons() const;

        NODISCARD BufferUsage GetUsage() const
        {
            return m_usage;
        }

        NODISCARD DrawMode GetDrawMode() const
        {
            return m_drawMode;
        }

        NODISCARD FrontFace GetFrontFace() const
        {
            return m_frontFace;
        }

        void ReArrangeIndex(FrontFace frontFace);


    private:
        //! Finalizing the buffer generation based on all the provided data.
        virtual void Build_Impl() = 0;

    protected:
        uint32 m_handle;
        bool m_hasBuilt = false;
        BufferUsage m_usage = BufferUsage::StaticDraw;
        FrontFace m_frontFace = FrontFace::CounterClockWise;
        DrawMode m_drawMode = DrawMode::Triangle;
        std::vector<uint32> m_indices;
    };
}