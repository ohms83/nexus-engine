//
//  Vertex.hpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#ifndef __NXS_VERTEX_H__
#define __NXS_VERTEX_H__

#include "NxsMacros.h"
#include "Color.hpp"

#include "glm/glm.hpp"

#include <vector>

NXS_NAMESPACE {
    /**
     * A vertex that composes of position, normal, and texture coordinates.
     * This is commonly used for 3D models.
     */
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        std::vector<glm::vec2> textCoords;
        uint32_t textID;
    };
    /**
     * A vertext that composes of only position and color data.
     */
    struct VertexC
    {
        glm::vec3 position;
        Color color;
    };
    /**
     * A vertext that composes of position, color and texture coordinates.
     */
    struct VertexCT
    {
        glm::vec3 position;
        Color color;
        std::vector<glm::vec2> textCoords;
        uint32_t textID;
    };
}

#endif /* __NXS_VERTEX_H__ */
