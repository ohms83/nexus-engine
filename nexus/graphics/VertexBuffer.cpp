#include "VertexBuffer.hpp"

#include <map>

USING_NAMESPACE_NXS;

static std::map<VertexBuffer::Stride, uint32_t> STRIDE_ELEMENT_COUNT = {
    { VertexBuffer::P3_N3_T2_C4, 12 },
    { VertexBuffer::P3_T2_C4,     9 },
    { VertexBuffer::P2_T2_C4,     8 },
    { VertexBuffer::P3_C4,        7 },
    { VertexBuffer::P2_C4,        6 },
};

static std::map<VertexBuffer::Stride, uint32_t> STRIDE_SIZE_MAP = {
    { VertexBuffer::P3_N3_T2_C4, sizeof(float) * 12 },
    { VertexBuffer::P3_T2_C4,    sizeof(float) *  9 },
    { VertexBuffer::P2_T2_C4,    sizeof(float) *  8 },
    { VertexBuffer::P3_C4,       sizeof(float) *  7 },
    { VertexBuffer::P2_C4,       sizeof(float) *  6 },
};

uint32_t VertexBuffer::getStrideSize(Stride stride)
{
    return STRIDE_SIZE_MAP[stride];
}

uint32_t VertexBuffer::getStrideElementCount(Stride stride)
{
    return STRIDE_ELEMENT_COUNT[stride];
}

uint32_t VertexBuffer::getVertexOffset(Stride stride, uint32_t vertexIndex)
{
    return STRIDE_ELEMENT_COUNT[stride] * vertexIndex;
}

void VertexBuffer::allocateBuffer(uint32_t vertexCount, Stride stride)
{
    _buffer = std::unique_ptr<float[]>(new float[getVertexOffset(stride, vertexCount)]);
    _vertexCount = vertexCount;
    _stride = stride;
}

glm::vec3 VertexBuffer::getPosition(uint32_t vertextIndex) const
{
    if (_stride == Stride::NONE || _buffer == nullptr || _vertexCount == 0) {
        throw std::runtime_error("Failed to get vertex's position. REASON=Unitialized vertex buffer");
    }
    
    glm::vec3 pos;

    if (vertextIndex <= _vertexCount)
    {
        uint32_t offset = getVertexOffset(_stride, vertextIndex);
        pos.x = _buffer[offset];
        pos.y = _buffer[offset+1];

        if (_stride != P2_T2_C4 && _stride != P2_C4) {
            pos.z = _buffer[offset+2];
        }
    }

    return pos;
}