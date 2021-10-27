//
//  GLVertexBuffer.hpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#ifndef __NXS_GL_VERTEX_BUFFER_H__
#define __NXS_GL_VERTEX_BUFFER_H__

#include "NxsGL.h"
#include "graphics/VertexBuffer.hpp"

NXS_NAMESPACE {
    class GLVertexBuffer : public VertexBuffer
    {
    public:
        ~GLVertexBuffer();
        void initImpl(const VertexBufferCreateInfo& info) override;
        
        GLuint getVBO() const
        {
            return _vao;
        }
        
        GLuint getEBO() const
        {
            return _ebo;
        }
        
        GLuint getVAO() const
        {
            return _vao;
        }
        
        void updateVertexData(float* vertices) override;
        void updateIndexData(uint32_t* indices) override;
        
    private:
        GLuint _vbo = 0;
        GLuint _vao = 0;
        GLuint _ebo = 0;
    };
}

#endif /* __NXS_GL_VERTEX_BUFFER_H__ */
