//
//  GraphicsConst.cpp
//  Nexus
//
//  Created by nuttachai on 22/10/21.
//

#include "GraphicsConst.h"

namespace nexus
{
    int getVertexPerPrimitive(int primitiveType)
    {
        int vertexPerPrimitive = 0;
        
        switch(primitiveType)
        {
            case TRIANGLES:
            case TRIANGLE_STRIP:
            case TRIANGLE_FAN:
                vertexPerPrimitive = 3;
                break;
            case QUADS:
            case QUAD_STRIP:
                vertexPerPrimitive = 4;
                break;
        }
        
        return vertexPerPrimitive;
    }

    int getPolygonCount(int primitiveType, uint32_t vertexCount)
    {
        int vertexPerPrimitive = getVertexPerPrimitive(primitiveType);
        return (vertexPerPrimitive > 0 ? vertexCount / vertexPerPrimitive : 0);
    }
}
