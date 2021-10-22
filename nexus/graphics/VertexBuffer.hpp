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
        uint32_t vertexCount;
        float* vertices;
        uint32_t indexCount;
        uint32_t* indices;
        VertexDescription description;
    };
    
//    template<typename _VertexType>
//    class VertexBuffer
//    {
//    public:
//        VertexBuffer() {}
//
//        virtual void init(const VertexBufferCreateInfo& info)
//        {
//            if (!isInit()) {
//                _usage = info.usage;
//                _numTexCoord = info.numTexCoord;
//
//                if (info.reserveSize != 0) {
//                    vertices.reserve(info.reserveSize);
//                }
//            }
//            else {
//                // TODO: Error message
//            }
//        }
//
//        virtual void release()
//        {
//            if (isInit()) {
//                _usage = UNDEFINED;
//            }
//        }
//
//        bool isInit() const {
//            return _usage != UNDEFINED;
//        }
//
//        int getUsage() const {
//            return _usage;
//        }
//
//        uint32_t getBufferSize() const {
//            return (uint32_t)vertices.size() * sizeof(_VertexType);
//        }
//
//    public:
//        /// Vertex buffer data
//        std::vector<_VertexType> vertices;
//        /// Index buffer
//        std::vector<uint32_t> indices;
//
//    protected:
//        int _usage = UNDEFINED;
//        uint32_t _numTexCoord = 0;
//    };
    
    class VertexBuffer
    {
    public:
        void init(const VertexBufferCreateInfo& info);
        
        bool isInit() const {
            return _usage != UNDEFINED;
        }
        
    protected:
        /// Child class's implementation of init function.
        virtual void initImpl(const VertexBufferCreateInfo& info) = 0;
        
    protected:
        int _usage = UNDEFINED;
        uint32_t _vertexCount = 0;
        uint32_t _indexCount = 0;
        VertexDescription _description;
    };
}

#endif // __NXS_VERTEX_BUFFER_H__
