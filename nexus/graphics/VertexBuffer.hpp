#ifndef __NXS_VERTEX_BUFFER_H__
#define __NXS_VERTEX_BUFFER_H__

#include "nxsMacros.h"
#include "glm/glm.hpp"

#include <memory>

NXS_NAMESPACE {
    class VertexBuffer
    {
    public:
        enum Stride
        {
            NONE = -1,
            /**
             * Position and normal as 3D vectors, texture coordinate as 2D a vector, and an RGBA color.
             * This is commonly used for 3D models.
             */
            P3_N3_T2_C4,
            /**
             * Position as a 3D vector, texture coordinate as 2D a vector, and an RGBA color with no normal.
             */
            P3_T2_C4,
            /**
             * Position a 3D vector, and an RGBA color.
             */
            P3_C4,
            /**
             * Position and texture coordinate as 2D vectors, and an RGBA color.
             * This is commonly used for 2D sprites or UIs.
             */
            P2_T2_C4,
            /**
             * Position a 2D vector, and an RGBA color.
             */
            P2_C4,
        };

        void allocateBuffer(uint32_t vertexCount, Stride stride);

        void setPosition(uint32_t vertextIndex, const glm::vec3 position) const;
        glm::vec3 getPosition(uint32_t vertextIndex) const;

        static uint32_t getStrideSize(Stride stride);
        static uint32_t getStrideElementCount(Stride stride);
        static uint32_t getVertexOffset(Stride stride, uint32_t vertexIndex);

    private:
        std::unique_ptr<float[]> _buffer;
        uint32_t _vertexCount = 0;
        Stride _stride = Stride::NONE;
    };
}

#endif // __NXS_VERTEX_BUFFER_H__