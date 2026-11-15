#pragma once

#include <SDL3/SDL.h>

namespace nxs
{    
    using WindowContext = void*;

    struct RenderContext
    {
        void* nativeContext = nullptr; // Holds SDL_GLContext, VkDevice, ID3D12Device*, etc.
        void* nativeSurface = nullptr; // Holds VkSurfaceKHR, IDXGISwapChain*, etc.
    };
}