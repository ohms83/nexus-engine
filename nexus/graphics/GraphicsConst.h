#ifndef __NXS_GRAPHCIS_CONST_H__
#define __NXS_GRAPHCIS_CONST_H__

#include "NxsMacros.h"

NXS_NAMESPACE {
    const int RENDER_SYSTEM_GL              = 1;
    const int RENDER_SYSTEM_VULKAN          = 2;
    
    const int UNDEFINED                     = 0xFFFF;
    const int STREAM_DRAW                   = 0x88E0;
    const int STREAM_READ                   = 0x88E1;
    const int STREAM_COPY                   = 0x88E2;
    const int STATIC_DRAW                   = 0x88E4;
    const int STATIC_READ                   = 0x88E5;
    const int STATIC_COPY                   = 0x88E6;
    const int DYNAMIC_DRAW                  = 0x88E8;
    const int DYNAMIC_READ                  = 0x88E9;
    const int DYNAMIC_COPY                  = 0x88EA;
}

#endif // __NXS_GRAPHCIS_CONST_H__
