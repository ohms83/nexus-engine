//
// Created by nutta on 7/6/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"

#include <vector>

#include "GPUBuffer.h"
#include "GpuResource.h"
#include "GraphicsConst.h"

NXS_NAMESPACE
{
    class IndexBuffer : public IGpuResource
    {
    public:
        IndexBuffer() = default;

        uint32_t GetHandle() const override
        {
            return m_handle;
        }

        virtual IndexBuffer& Begin();
        virtual IndexBuffer& SetIndices(Ref<IBuffer> indexData, FrontFace frontFace);
        virtual IndexBuffer& SetUsage(BufferUsage usage);
        virtual IndexBuffer& SetDrawMode(DrawMode mode);
        virtual void Build();

        NODISCARD uint32_t NumIndex() const;
        NODISCARD uint32_t NumPolygons() const;

        NODISCARD const uint32_t* GetData() const
        {
            return R_CAST<const uint32_t*>(m_indexData->Data());
        }

        NODISCARD uint32_t GetBufferSize() const
        {
            return m_indexData->Size();
        }

        /**
         * @brief Specify the number of index to be rendered.
         * 
         * @param count Index count
         */
        void SetNumIndexDraw(uint32_t count);
        NODISCARD uint32_t GetNumIndexDraw() const;

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

        void CopyIndices(const std::vector<uint32_t>& indices, size_t offset = 0);
        void CopyData(const void* data, size_t bytes, size_t offset = 0) override;

    private:
        //! Finalizing the buffer generation based on all the provided data.
        virtual void Build_Impl() = 0;

    protected:
        uint32_t m_handle = 0;
        uint32_t m_numIndex = 0;
        /**
         * @brief The number of index that's will be rendered.
         * 
         * The render command uses this value to tell the underlying render system how many indices will be rendered.
         * This is equal to @c m_numIndex by default unless it was changed by the @c SetNumIndexDraw function.
         */
        uint32_t m_numIndexDraw = 0;
        bool m_hasBuilt = false;
        BufferUsage m_usage = BufferUsage::StaticDraw;
        FrontFace m_frontFace = FrontFace::CounterClockWise;
        DrawMode m_drawMode = DrawMode::Triangle;
        Ref<IBuffer> m_indexData;
    };
}