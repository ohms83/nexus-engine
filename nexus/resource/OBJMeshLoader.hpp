//
//  OBJMesh.hpp
//  Nexus
//
//  Created by nuttachai on 4/11/21.
//

#pragma once

#include "NxsDefine.h"
#include <string>

NXS_NAMESPACE {
    class Mesh;
    
    class OBJMeshLoader
    {
    public:
        /// Load specified OBJ file and create a @c Mesh object.
        static Ref<Mesh> create(const std::string& path);
    };
}
