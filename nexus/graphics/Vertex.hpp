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
#include <cstdio>
#include <cstddef>
#include <vector>

NXS_NAMESPACE {
    /**
     * @c VertexDescription provides information about vertex data's memory layout.
     */
    struct VertexDescription
    {
        uint32_t positionComponentCount;
        size_t positionOffset;
        
        uint32_t colorComponentCount;
        size_t colorOffset;
        
        uint32_t normalComponentCount;
        size_t normalOffset;
        
        uint32_t texCoordComponentCount;
        uint32_t textureUnit;
        std::vector<size_t> texCoordOffsets;
        
        /// Return size of the vertex in bytes
        size_t getVertexSize() const;
    };
    
    template<size_t _ColorUnit, size_t _NormalUnit, size_t _TextureUnit>
    class Vertex_t
    {
    public:
        static_assert(_NormalUnit <= 1, "_NormalUnit can only be either 0 or 1.");
        static_assert(_ColorUnit <= 1, "_ColorUnit can only be either 0 or 1.");
        
        Vertex_t()
        {
        }
        
        glm::vec3& position() {
            return _position;
        }
        
        glm::vec4& color() {
            static_assert(_ColorUnit > 0, "Vertex's color is inaccesible because it doesn't exist!");
            return _color[0];
        }
        
        glm::vec3& normal() {
            static_assert(_NormalUnit > 0, "Vertex's normal is inaccesible because it doesn't exist!");
            return _normals[0];
        }
        
        glm::vec3& texCoord(const size_t index) {
            static_assert(_TextureUnit > 0, "Vertex's texture coordinate is inaccesible because it doesn't exist!");
            if (index >= _TextureUnit) {
                char log[64] = {};
                snprintf(log, sizeof(char) * 64, "Texture's index out of range (%d/%d)!", (int)index, (int)_TextureUnit);
                throw std::runtime_error(log);
            }
            return _texCoords[index];
        }
        
        static size_t getPositionOffset()
        {
            return offsetof(Vertex_t, _position);
        }
        
        static size_t getColorOffset()
        {
            if (_ColorUnit > 0) {
                return offsetof(Vertex_t, _color);
            }
            else {
                return 0;
            }
        }
        
        static size_t getNormalOffset()
        {
            if (_NormalUnit > 0) {
                return offsetof(Vertex_t, _normals);
            }
            else {
                return 0;
            }
        }
        
        static size_t getTexCoordOffset(const size_t index)
        {
            if (_TextureUnit > 0 && index < _TextureUnit) {
                return (offsetof(Vertex_t, _texCoords) + (sizeof(glm::vec2) * index));
            }
            else {
                return 0;
            }
        }
        
        static VertexDescription getDescription()
        {
            VertexDescription desc = {
                // Position
                3, getPositionOffset(),
                // Color
                4 * _ColorUnit, getColorOffset(),
                // Normal
                3 * _NormalUnit, getNormalOffset(),
                // Texture Coordinates
                2 * _TextureUnit, _TextureUnit
            };
            
            for (auto i = 0; i < _TextureUnit; ++i) {
                desc.texCoordOffsets.emplace_back(getTexCoordOffset(i));
            }
            
            return desc;
        }
        
        static const size_t MAX_TEXTURE_UNIT = _TextureUnit;
        
    protected:
        glm::vec3 _position;
        glm::vec4 _color[_ColorUnit];
        glm::vec3 _normals[_NormalUnit];
        glm::vec2 _texCoords[_TextureUnit];
    };
    /**
     * A vertex that composes of position, normal, and one texture coordinates.
     * This is commonly used for 3D models.
     * @note This vertex doesn't contain color channel; thus, cannot be tinted or painted.
     *       Please use @c ColoredVertex3D for those purposes.
     */
    typedef Vertex_t<0, 1, 1> Vertex3D;
    /**
     * A vertex that composes of position, color, normal, and one texture coordinates.
     */
    typedef Vertex_t<1, 1, 1> ColoredVertex3D;
    /**
     * A vertex that composes of position, color, and one texture coordinates.
     * This is commonly used for 2D sprites and UIs.
     * @note Unlike @c Vertex3D , Vertex2D is paintable by default.
     */
    typedef Vertex_t<1, 0, 1> Vertex2D;
}

#endif /* __NXS_VERTEX_H__ */
