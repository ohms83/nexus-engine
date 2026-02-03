#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"
#include "nexus/core/serialize/Serializeable.h"

#include "GPUBuffer.h"
#include "GpuResource.h"

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
        int32_t numElements;
        //! Optional explicit attribute index. If negative, Type enum is used.
        int32_t attribIndex = -1;
        //! Optional attribute divisor for instanced attributes (0 = per-vertex, 1 = per-instance)
        int32_t divisor = 0;

        static const VertexAttribute VertexPosition3D;
        static const VertexAttribute VertexPosition2D;
        static const VertexAttribute VertexNormal;
        static const VertexAttribute VertexColor0;
        static const VertexAttribute VertexColor1;
        static const VertexAttribute VertexTangent;
        static const VertexAttribute VertexBinormal;
        static const VertexAttribute VertexTexCoord0;
        static const VertexAttribute VertexTexCoord1;
        static const VertexAttribute VertexTexCoord2;
        static const VertexAttribute VertexTexCoord3;
        static const VertexAttribute VertexTexCoord4;
        static const VertexAttribute VertexTexCoord5;
        static const VertexAttribute VertexTexCoord6;
        static const VertexAttribute VertexTexCoord7;
    };

    struct VertexInstanceStream
    {
        Ref<IBuffer> buffer;
        std::vector<VertexAttribute> attributes;
        BufferUsage usage;
        uint32_t stride;
    };

    class VertexBuffer : public IGpuResource
    {
    public:
        /**
         * @brief An iterator used for iterating over the list of vertices
         * 
         * @tparam T Type of vertex. This must have the same memory layout as the data described by
         * the vertex attributes. 
         */
        template<class T>
        class Iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            explicit Iterator(const VertexBuffer& vertexBuffer)
            {
                m_current = R_CAST<T*>(vertexBuffer.GetData());
            }

            explicit Iterator(const VertexBuffer& vertexBuffer, size_t offset)
            {
                m_current = R_CAST<T*>(vertexBuffer.GetData() + offset);
            }

            Iterator(const Iterator& rhs) : m_current(rhs.m_current) {}

            /**
             * @brief Pre-increment operator.
             * @return This vertex iterator.
             */
            Iterator& operator++()
            {
                ++m_current;
                return *this;
            }
            /**
             * @brief Post-increment operator.
             * @return This vertex iterator.
             */
            Iterator operator++(int) { Iterator orig(*this); operator++(); return orig; }

            bool operator==(const Iterator& rhs) const { return m_current == rhs.m_current; }
            bool operator!=(const Iterator& rhs) const { return m_current != rhs.m_current; }
            NODISCARD T& operator*() { return *m_current; }
            NODISCARD const T& operator*() const { return *m_current; }
        private:
            T* m_current = nullptr;
        };

        VertexBuffer() = default;

        uint32_t GetHandle() const override
        {
            return m_handle;
        }

        /**
         * Start building this vertex buffer.
         * TODO: Rename this to make it less ambigious.
         */
        virtual VertexBuffer& Begin();
        virtual VertexBuffer& SetVertices(Ref<IBuffer> vertexData);
        virtual VertexBuffer& SetUsage(BufferUsage usage);
        virtual VertexBuffer& AddAttribute(const VertexAttribute& attribute);
        //! Add an instance data stream with its attributes. The buffer holds per-instance data.
        virtual VertexBuffer& AddInstanceStream(Ref<IBuffer> instanceData, const std::vector<VertexAttribute>& attributes, BufferUsage usage = BufferUsage::StaticDraw);
        virtual VertexBuffer& AttachInstanceStream(Ref<IBuffer> instanceData, const std::vector<VertexAttribute>& attributes, BufferUsage usage = BufferUsage::StaticDraw);
        virtual void DetachInstanceStreams();
        void Build();

        NODISCARD const std::vector<VertexInstanceStream>& GetInstanceStreams() const { return m_instanceStreams; }

        NODISCARD uint32_t GetStride() const
        {
            return m_stride;
        }

        NODISCARD BufferUsage GetUsage() const
        {
            return m_usage;
        }

        NODISCARD const std::vector<VertexAttribute>& GetAttributes() const
        {
            return m_attributes;
        }

        NODISCARD const VertexAttribute* FindAttribute(VertexAttribute::Type type) const
        {
            for (const auto& attr : m_attributes)
            {
                if (attr.type == type)
                {
                    return &attr;
                }
            }
            return nullptr;
        }

        NODISCARD uint32_t GetAttributeOffset(VertexAttribute::Type type) const;

        template<typename T>
        std::vector<T> GetVertices() const
        {
            static_assert(sizeof(T) == m_stride);

            std::vector<T> result;
            T* vertex = reinterpret_cast<T*>(m_vertices->Data());
            for (uint32_t index = 0; index < m_vertexCount; index++)
            {
                result.push_back(vertex[index]);
            }
            return result;
        }

        uint32_t VertexCount() const
        {
            return m_vertexCount;
        }

        uint8_t* GetData() const
        {
            return m_vertices->Data();
        }

        size_t GetBufferSize() const
        {
            return m_stride * m_vertexCount;
        }

        // ---- STD Container compliance ----

        template<class T>
        Iterator<T> begin() const
        {
            return Iterator<T>(*this);
        }

        template<class T>
        Iterator<T> end() const
        {
            return Iterator<T>(*this, m_stride * m_vertexCount);
        }

        /**
         * @brief Get the number of vertex this buffer is holding.
         * 
         * @return size_t The number of vertex.
         * @warning This function is just an alias of @c VertexCount . It's here as STD container compliance.
         * If you want to get the size of the buffer in bytes, please use @c GetBufferSize
         * @see GetBufferSize
         */
        size_t size() const
        {
            return VertexCount();
        }

    private:
        //! API specific vertex buffer generation function.
        virtual void Build_Impl() = 0;

    protected:
        uint32_t m_handle = 0;
        bool m_hasBuilt = false;
        uint32_t m_stride = 0;
        uint32_t m_vertexCount = 0;
        Ref<IBuffer> m_vertices;
        BufferUsage m_usage = BufferUsage::StaticDraw;
        std::vector<VertexAttribute> m_attributes;
        std::vector<VertexInstanceStream> m_instanceStreams;
    };
}