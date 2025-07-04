#pragma once

#include <array>
#include <vector>

#include <nexus/NxsDefine.h>

#include "GraphicsBuffer.h"
#include "memory/Buffer.h"

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
        VertexBuffer();
        virtual ~VertexBuffer() = default;

        /**
         *   Start building this vertex buffer. This must be called before,
         *   @anchor AddAttribute @anchor SetUsage and @anchor Build.
         */
        VertexBuffer& Begin();
        VertexBuffer& AddAttribute(const VertexAttribute& attribute);
        VertexBuffer& SetUsage(BufferUsage usage);
        VertexBuffer& SetVertexData(Buffer&& buffer);
        void Build();

        uint32 GetStride() const
        {
            return m_stride;
        }

        virtual void Bind() = 0;

    protected:
        //! API specific vertex buffer generation function.
        virtual void Build_Impl() = 0;

    protected:
        bool m_hasBuilt = false;
        int32 m_handle = -1;
        uint32 m_stride = 0;
        uint32 m_vertexCount = 0;
        Buffer m_vertexData;
        BufferUsage m_usage = BufferUsage::Static;
        std::array<int16, SIZE_CAST(VertexAttribute::Type::Num)> m_attribOffsets;
    };
}