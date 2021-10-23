//
//  GLRenderCommand.hpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#ifndef __NXS_GL_RENDER_COMMAND_H__
#define __NXS_GL_RENDER_COMMAND_H__

#include "graphics/RenderCommand.hpp"

NXS_NAMESPACE {
    class GLRenderCommand : public RenderCommand
    {
    public:
        GLRenderCommand();
        virtual ~GLRenderCommand();
        
        void execute() const override;
    };
}

#endif /* __NXS_GL_RENDER_COMMAND_H__ */
