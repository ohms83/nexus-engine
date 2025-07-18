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
            Position,   // 0
            Normal,     // 1
            Color0,     // 2
            Color1,     // 3
            Tangent,    // 4
            Binormal,   // 5
            TexCoord0,  // 6
            TexCoord1,  // 7
            TexCoord2,  // 8
            TexCoord3,  // 9
            TexCoord4,  // 10
            TexCoord5,  // 11
            TexCoord6,  // 12
            TexCoord7,  // 13
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

        NODISCARD uint32 GetStride() const
        {
            return m_stride;
        }

        template<typename T>
        std::vector<T> GetVertices() const
        {
            std::vector<T> result;
            T* vertex = reinterpret_cast<T*>(m_vertices.get());
            for (uint32 index = 0; index < m_vertexCount; index++)
            {
                result.push_back(vertex[index]);
            }
            return result;
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