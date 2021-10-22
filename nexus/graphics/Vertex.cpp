//
//  Vertex.cpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#include "Vertex.hpp"

USING_NAMESPACE_NXS;

size_t VertexDescription::getVertexSize() const
{
    const size_t componentCount = positionComponentCount + normalComponentCount + colorComponentCount + (texCoordComponentCount * textureUnit);
    return (sizeof(float) * componentCount);
}
