#pragma once

#include <nexus/NxsDefine.h>
#include <vector>

#include "GPUBuffer.h"
#include <nexus/memory/Buffer.h>

#include "GpuResrouce.h"

NXS_NAMESPACE
{
    struct VertexAttribute
    {
        enum class Type
        {
            Position,
            Normal,
            Color0,
            Color1,
            Tangent,
            Binormal,
            TexCoord0,
            TexCoord1,
            TexCoord2,
            TexCoord3,
            TexCoord4,
            TexCoord5,
            TexCoord6,
            TexCoord7,
            Num
        };

        Type type;
        DataType dataType;
        int32 numElements;
    };

    class VertexBuffer : public GpuResrouce
    {
    public:
        VertexBuffer() = default;

        /**
         * Start building this vertex buffer.
         */
        virtual VertexBuffer& Begin();
        virtual VertexBuffer& SetVertices(const uint8* vertexData, size_t size);
        virtual VertexBuffer& SetUsage(BufferUsage usage);
        virtual VertexBuffer& AddAttribute(const VertexAttribute& attribute);
        void Build();

        [[nodiscard]] uint32 GetStride() const
        {
            return m_stride;
        }

    private:
        //! API specific vertex buffer generation function.
        virtual void Build_Impl() = 0;

    protected:
        bool m_hasBuilt = false;
        uint32 m_stride = 0;
        uint32 m_vertexCount = 0;
        std::unique_ptr<uint8[]> m_vertices;
        size_t m_bufferSize = 0;
        BufferUsage m_usage = BufferUsage::StaticDraw;
        std::vector<VertexAttribute> m_attributes;
    };
}