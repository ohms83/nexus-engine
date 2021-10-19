#include "VertexBuffer.hpp"

#include <map>

USING_NAMESPACE_NXS;

uint32_t VertexBuffer::getVertexSize(const VertexAlignment& align)
{
    return getNumComponent(align) * sizeof(float);
}

uint32_t VertexBuffer::getNumComponent(const VertexAlignment& align)
{
    uint32_t componentCount = align.positionComponent + align.colorComponent + align.normalComponent;
    for (const auto texCoordComponent : align.texCoordComponents) {
        componentCount += texCoordComponent;
    }

    return componentCount;
}

void VertexBuffer::allocate(uint32_t vertexCount, const VertexAlignment& align)
{
    _buffer = std::unique_ptr<float[]>(new float[getNumComponent(align)]);
    _vertexCount = vertexCount;
    _alignment = align;
}

void VertexBuffer::take(float* buffer, uint64_t size, const VertexAlignment& align)
{
    _buffer = std::unique_ptr<float[]>(buffer);
    _vertexCount = (uint32_t)(size / getVertexSize(align));
    _alignment = align;
}

void VertexBuffer::take(Data& data, const VertexAlignment& align)
{
    uint64_t size = 0;
    _buffer = std::unique_ptr<float[]>(reinterpret_cast<float*>(data.give(size)));
    _vertexCount = (uint32_t)(size / getVertexSize(align));
    _alignment = align;
}

glm::vec3 VertexBuffer::getPosition(uint32_t vertextIndex) const
{
    return glm::vec3();
}