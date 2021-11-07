//
//  Mesh.hpp
//  Nexus
//
//  Created by nuttachai on 31/10/21.
//

#pragma once

#include "NxsDefine.h"
#include "graphics/GraphicsConst.h"
#include "graphics/VertexBuffer.hpp"

#include <vector>

NXS_NAMESPACE {
    class MeshBuilder;
    class RenderSystem;
    
    class Mesh
    {
    public:
        friend class MeshBuilder;
        
        /**
         * @param primitiveType Must be one of the following:
         *  - POINTS
         *  - LINES
         *  - TRIANGLES
         *  - QUADS
         * @param usage Giving a hint to the engine about how the mesh should be optimized.
         *              Must be one of the following: STATIC_DRAW, DYNAMIC_DRAW and STREAM_DRAW.
         * @see GraphicsConst.h
         */
        void init(int primitiveType, int usage, Ref<VertexBuffer> vertexBuffer, const VertexDescription& vertexDesc);
        
        const int getPrimitiveType() const {
            return _primitiveType;
        }
        
        const int getUsageFlag() const {
            return _usage;
        }
        
        const std::vector<float>& getVertices() const;
        std::vector<float>& getVertices();
        
        const std::vector<uint32_t>& getIndices() const;
        std::vector<uint32_t>& getIndices();
        
        static int getPrimitiveFromVertexCount(int count);
        
    protected:
        Mesh();
        
    protected:
        int _primitiveType = UNDEFINED;
        int _usage = UNDEFINED;
        VertexDescription _vertexDesc {};
        std::vector<float> _vertices;
        std::vector<uint32_t> _indices;
        Ref<VertexBuffer> _vertexBuffer;
    };
    
    class MeshBuilder
    {
    public:
        /**
         * VertexComponent defines attributes of a vertex.
         * The attribute's index starts from 1 while the index 0 means vertex doesn't have
         * that attribute.
         * For example, { 1, 2, 5 } means vertex's position, normal and texture coordinates
         * data are drawn from their coresponding arrays at index 0, 1, and 4 respectively.
         */
        struct VertexComponent
        {
            uint32_t positionIndex;
            uint32_t normalIndex;
            uint32_t texCoordIndex;
            
            bool operator == (const VertexComponent& rhs) const {
                return std::tie(positionIndex, normalIndex, texCoordIndex) == std::tie(rhs.positionIndex, rhs.normalIndex, rhs.texCoordIndex);
            }
            
            bool operator < (const VertexComponent& rhs) const {
                return std::tie(positionIndex, normalIndex, texCoordIndex) < std::tie(rhs.positionIndex, rhs.normalIndex, rhs.texCoordIndex);
            }
        };
        
        Ref<Mesh> create();
        
        MeshBuilder& addPosition(const glm::vec3& pos);
        MeshBuilder& addNormal(const glm::vec3& normal);
        MeshBuilder& addTexCoord(const glm::vec2& texCoord);
        /// Construct a polygon face from the specified list of vertices
        MeshBuilder& addPolygon(const std::initializer_list<VertexComponent>& vertices);
        /// Construct a polygon face from the specified list of vertices
        MeshBuilder& addPolygon(const std::vector<VertexComponent>& vertices);
        
        template<typename _Iterator>
        MeshBuilder& addPolygon(const _Iterator& begin, const _Iterator& end)
        {
            for (_Iterator current = begin; current != end; ++current)
            {
                VertexComponent vertex = *current;
                uint32_t index = 0;
                const auto itr = std::find(_vertices.begin(), _vertices.end(), vertex);
                
                if (itr == _vertices.end())
                {
                    index = (uint32_t)_vertices.size();
                    _vertices.emplace_back(vertex);
                }
                else
                {
                    index = (uint32_t)(itr - _vertices.begin());
                }
                
                _indices.emplace_back(index);
            }
            return *this;
        }
        
    private:
        std::vector<glm::vec3> _positions;
        std::vector<glm::vec3> _normals;
        std::vector<glm::vec2> _texCoords;
        std::vector<VertexComponent> _vertices;
        /**
         * A list of vertex indices that making up a polygon.
         * For example, [1, 2, 3] means this triangle consists of three vertices
         * from index 1, 2 and 3.
         */
        std::vector<uint32_t> _indices;
    };
}
