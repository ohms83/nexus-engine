#ifndef __NXS_VERTEX_BUFFER_H__
#define __NXS_VERTEX_BUFFER_H__

#include "nxsMacros.h"
#include "glm/glm.hpp"

#include "io/Data.hpp"

#include <memory>
#include <array>

NXS_NAMESPACE {
    class VertexBuffer
    {
    public:
        /**
         * A data structure describing how vertex data is aligned within buffer.
         */
        struct VertexAlignment
        {
            /// How many component per position.
            uint32_t positionComponent;
            uint32_t positionOffset;

            /// How many component per normal.
            uint32_t normalComponent;
            uint32_t normalOffset;

            /// How many component per normal.
            uint32_t colorComponent;
            uint32_t colorOffset;

            /// How many component per normal.
            std::array<uint32_t, 4> texCoordComponents;
            std::array<uint32_t, 4> texCoordOffset;
        };
        void allocate(uint32_t vertexCount, const VertexAlignment& align);

        /**
         * Take over the specified @c buffer and manage it.
         * @warning buffer's ownership will be taken over by this object.
         *          Please DO NOT delete it.
         * @warning The pointer must be dynamically allocated on heap. Please DO NOT
         *          pass the stack's pointer to this function because it will cause
         *          memory corruption when this object is destroyed.
         */
        void take(float* buffer, uint64_t size, const VertexAlignment& align);

        /**
         * Take over the buffer from specified @c data .
         * @note The size of data must be divisible by the stride's size.
         * @warning After taking over, @c data will remain in undefined state and can't be reused.
         */
        void take(Data& data, const VertexAlignment& align);

        void setPosition(uint32_t vertextIndex, const glm::vec3 position) const;
        glm::vec3 getPosition(uint32_t vertextIndex) const;

        static uint32_t getVertexSize(const VertexAlignment& align);
        /// Return a total number of data component per vertex.
        static uint32_t getNumComponent(const VertexAlignment& align);

    private:
        std::unique_ptr<float[]> _buffer;
        uint32_t _vertexCount = 0;
        VertexAlignment _alignment;
    };
}

#endif // __NXS_VERTEX_BUFFER_H__