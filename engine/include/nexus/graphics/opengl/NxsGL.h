#pragma once

#define GL_GLEXT_PROTOTYPES
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <cassert>

#include <nexus/NxsDefine.h>

#define GL_STRICT_CHECK

#ifdef GL_STRICT_CHECK
    #define CHECK_GL_ERROR() {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            std::stringstream ss;\
            ss << "OPENGL ERROR! ERROR CODE=" << error;\
            std::cout << ss.str() << std::endl;\
            assert(false);\
        }\
    }
#else
    #define CHECK_GL_ERROR() {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            std::cout << "OPENGL ERROR! ERROR CODE=" << error << std::endl;\
        }\
    }
#endif
