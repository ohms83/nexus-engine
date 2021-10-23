//
//  RenderCommand.hpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#ifndef __NXS_RENDER_COMMAND_H__
#define __NXS_RENDER_COMMAND_H__

#include "NxsDefine.h"
#include "Color.hpp"

#include <vector>

NXS_NAMESPACE {
    class VertexBuffer;
    class Shader;
    
    class RenderCommand
    {
    public:
        RenderCommand();
        virtual ~RenderCommand();
        
        void addVertexBuffer(const VertexBuffer* vertexBuffer);
        void setShader(const Shader* shader);
        
        virtual void execute() const = 0;
        
    protected:
        /// A list of vertex buffer to draw.
        std::vector<const VertexBuffer*> _bufferList;
        const Shader* _shader = nullptr;
    };
}

#endif /* __NXS_RENDER_COMMAND_H__ */
