#pragma once

#include <SDL3/SDL.h>

namespace nxs
{    
    using WindowContext = SDL_Window*;

    union RenderContext
    {
        SDL_GLContext gl_context;
    };
}