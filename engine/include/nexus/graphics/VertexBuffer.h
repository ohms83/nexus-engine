#pragma once

#include <nexus/NxsDefine.h>
#include <vector>

#include "GPUBuffer.h"
#include <nexus/memory/Buffer.h>

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
        uint32 numElements;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer() = default;
        virtual ~VertexBuffer() = default;

        /**
         *   Start building this vertex buffer. This must be called before,
         *   @anchor AddAttribute @anchor SetUsage and @anchor Build.
         *   @param vertexData
         *   @param usage Buffer usage hint.
         */
        virtual VertexBuffer& Begin(Buffer&& vertexData, BufferUsage usage);
        virtual VertexBuffer& Begin(uint8* data, size_t size, BufferUsage usage);
        virtual VertexBuffer& AddAttribute(const VertexAttribute& attribute);
        void Build();

        uint32 GetStride() const
        {
            return m_stride;
        }

    protected:
        //! API specific vertex buffer generation function.
        virtual void Build_Impl() = 0;

    protected:
        bool m_hasBuilt = false;
        uint32 m_stride = 0;
        uint32 m_vertexCount = 0;
        Buffer m_vertexData;
        BufferUsage m_usage = BufferUsage::StaticDraw;
        std::vector<VertexAttribute> m_attributes;
    };
}