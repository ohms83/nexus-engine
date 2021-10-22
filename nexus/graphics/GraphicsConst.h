#ifndef __NXS_GRAPHCIS_CONST_H__
#define __NXS_GRAPHCIS_CONST_H__

#include "NxsMacros.h"

#include <cstdint>

NXS_NAMESPACE {
    const int RENDER_SYSTEM_GL              = 1;
    const int RENDER_SYSTEM_VULKAN          = 2;
    
    const int UNDEFINED                     = 0xFFFF;
    
    /* Buffer usages */
    const int STREAM_DRAW                   = 0x88E0;
    const int STREAM_READ                   = 0x88E1;
    const int STREAM_COPY                   = 0x88E2;
    const int STATIC_DRAW                   = 0x88E4;
    const int STATIC_READ                   = 0x88E5;
    const int STATIC_COPY                   = 0x88E6;
    const int DYNAMIC_DRAW                  = 0x88E8;
    const int DYNAMIC_READ                  = 0x88E9;
    const int DYNAMIC_COPY                  = 0x88EA;
    
    /* Primitives */
    const int POINTS                        = 0x0000;
    const int LINES                         = 0x0001;
    const int LINE_LOOP                     = 0x0002;
    const int LINE_STRIP                    = 0x0003;
    const int TRIANGLES                     = 0x0004;
    const int TRIANGLE_STRIP                = 0x0005;
    const int TRIANGLE_FAN                  = 0x0006;
    const int QUADS                         = 0x0007;
    const int QUAD_STRIP                    = 0x0008;
    
    int getVertexPerPrimitive(int primitiveType);
    int getPolygonCount(int primitiveType, uint32_t vertexCount);
}

#endif // __NXS_GRAPHCIS_CONST_H__
