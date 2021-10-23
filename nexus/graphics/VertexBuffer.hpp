#ifndef __NXS_VERTEX_BUFFER_H__
#define __NXS_VERTEX_BUFFER_H__

#include "NxsMacros.h"
#include "GraphicsConst.h"
#include "Vertex.hpp"
#include "io/Data.hpp"

#include <memory>
#include <vector>

NXS_NAMESPACE {
    /**
     * VertexBufferCreateInfo identifies vertex buffer's usage and properties.
     * - @c usage identifies vertex buffer usage. Can only be one of the following values.
     *   - nexus::STATIC_DRAW  The buffer will be optimized for read-only data.
     *   - nexus::DYNAMIC_DRAW The buffer will be optimized for read/write data.
     *   - nexus::STREAM_DRAW  The buffer will be optimized for read-only data that might not be accessed so often.
     */
    struct VertexBufferInfo
    {
        int usage;
        int primitiveType;
        uint32_t vertexCount;
        uint32_t indexCount;
        VertexDescription description;
    };
    
    struct VertexBufferCreateInfo
    {
        VertexBufferInfo bufferInfo;
        float* vertices;
        uint32_t* indices;
    };
    
    class VertexBuffer
    {
    public:
        VertexBuffer();
        virtual ~VertexBuffer();
        
        void init(const VertexBufferCreateInfo& info);
        
        bool isInit() const {
            return _info.usage != UNDEFINED;
        }
        
        const VertexBufferInfo& getInfo() const {
            return _info;
        }
        
        int getPolygonCount() const {
            return nexus::getPolygonCount(_info.primitiveType, _info.vertexCount);
        }
        
    protected:
        /// Child class's implementation of init function.
        virtual void initImpl(const VertexBufferCreateInfo& info) = 0;
        
    protected:
        VertexBufferInfo _info = { UNDEFINED, UNDEFINED, 0, 0, {} };
    };
}

#endif // __NXS_VERTEX_BUFFER_H__
