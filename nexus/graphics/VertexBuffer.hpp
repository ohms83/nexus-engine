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
    struct VertexBufferCreateInfo
    {
        int usage;
        int primitiveType;
        uint32_t vertexCount;
        float* vertices;
        uint32_t indexCount;
        uint32_t* indices;
        VertexDescription description;
    };
    
    class VertexBuffer
    {
    public:
        void init(const VertexBufferCreateInfo& info);
        
        bool isInit() const {
            return _usage != UNDEFINED;
        }
        
        uint32_t getVertexCount() const  {
            return _vertexCount;
        }
        
        uint32_t getIndexCount() const  {
            return _indexCount;
        }
        
        int getPrimitiveType() const {
            return _primitiveType;
        }
        
        int getPolygonCount() const {
            return nexus::getPolygonCount(_primitiveType, _vertexCount);
        }
        
    protected:
        /// Child class's implementation of init function.
        virtual void initImpl(const VertexBufferCreateInfo& info) = 0;
        
    protected:
        int _usage = UNDEFINED;
        int _primitiveType = UNDEFINED;
        uint32_t _vertexCount = 0;
        uint32_t _indexCount = 0;
        VertexDescription _description;
    };
}

#endif // __NXS_VERTEX_BUFFER_H__
