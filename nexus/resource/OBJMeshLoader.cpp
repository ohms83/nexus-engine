//
//  ObjMesh.cpp
//  Nexus
//
//  Created by nuttachai on 4/11/21.
//

#include "OBJMeshLoader.hpp"

#include "Mesh.hpp"
#include "base/CString.hpp"
#include "io/File.hpp"

#include <algorithm>

USING_NAMESPACE_NXS;

template<uint32_t L>
glm::vec<L, float, glm::defaultp> scanVector(std::vector<char*> tokens)
{
    glm::vec<L, float, glm::defaultp> result;
    if (tokens.size() >= L)
    {
        for (uint32_t i = 0; i < L; ++i) {
            sscanf((const char*)tokens[i], "%f", &result[i]);
        }
    }
    return result;
}

Ref<Mesh> OBJMeshLoader::create(const std::string& path)
{
    MeshBuilder builder;
    
    Data data = File::readData(path, File::TEXT);
    char* str = const_cast<char*>(data.get());
    
    CString::iterateLines(str, [&] (char* line) -> bool {
        line = CString::trim(line);
        
        // Skipping an empty line or a comment.
        if (!line || strlen(line) == 0 || line[0] == '#') {
            return true;
        }
        
        std::vector<char*> tokens = CString::split(line, " ", true);
        if (tokens.empty()) {
            return true;
        }
        
        if (tokens[0][0] == 'v')
        {
            // Parsing vertex's position
            builder.addPosition(scanVector<3>(tokens));
        }
        else if (strncmp(tokens[0], "vn", 2))
        {
            // Parsing vertex's normal
            builder.addNormal(scanVector<3>(tokens));
        }
        else if (strncmp(tokens[0], "vt", 2))
        {
            // Parsing texture coordinate
            builder.addTexCoord(scanVector<2>(tokens));
        }
        else if (tokens[0][0] == 'f')
        {
            // Parsing polygon face
            const size_t numToken = tokens.size();
            std::vector<MeshBuilder::VertexComponent> vetices;

            for (size_t i = 1; i < numToken; ++i)
            {
                MeshBuilder::VertexComponent vertex {};
                std::vector<char*> elementStr = CString::split(tokens[i], "/");
                if (elementStr.size() < 3) {
                    // TODO: Error message
                    return false;
                }

                sscanf("%u", elementStr[0], &vertex.positionIndex);
                sscanf("%u", elementStr[1], &vertex.texCoordIndex);
                sscanf("%u", elementStr[2], &vertex.normalIndex);

                vetices.emplace_back(vertex);
            }

            builder.addPolygon(vetices);
        }
        
        return true;
    });
    
    return builder.create();
}
