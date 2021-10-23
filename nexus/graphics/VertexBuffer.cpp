#include "VertexBuffer.hpp"

USING_NAMESPACE_NXS;

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::init(const VertexBufferCreateInfo& info)
{
    if (!isInit())
    {
        _info = info.bufferInfo;
        initImpl(info);
    }
    else
    {
        // TODO: Error message
    }
}
