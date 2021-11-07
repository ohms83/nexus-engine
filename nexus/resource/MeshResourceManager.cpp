//
//  MeshResourceManager.cpp
//  Nexus
//
//  Created by nuttachai on 1/11/21.
//

#include "MeshResourceManager.hpp"
#include "OBJMeshLoader.hpp"

#include "io/File.hpp"
#include "os/Path.hpp"
#include "base/String.hpp"

#include <cstring>
#include <tuple>

USING_NAMESPACE_NXS;

Ref<Mesh> MeshResourceManager::create(const std::string& filePath)
{
    Ref<Mesh> result;
    Path path(filePath);
    auto [head, tail] = path.splitExt();
    const std::string& ext = String::tolower(tail);
    
    if (ext == ".obj")
    {
        result = OBJMeshLoader::create(filePath);
    }
    
    return result;
}
