#include "VertexBuffer.hpp"

USING_NAMESPACE_NXS;

void VertexBuffer::init(const VertexBufferCreateInfo& info)
{
    if (!isInit())
    {
        _usage          = info.usage;
        _vertexCount    = info.vertexCount;
        _indexCount     = info.indexCount;
        _description    = info.description;
        
        initImpl(info);
    }
    else
    {
        // TODO: Error message
    }
}
