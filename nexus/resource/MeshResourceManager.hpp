//
//  MeshResourceManager.hpp
//  Nexus
//
//  Created by nuttachai on 1/11/21.
//

#ifndef __NXS_MESH_RESOURCE_MANAGER_H__
#define __NXS_MESH_RESOURCE_MANAGER_H__

#include "NxsDefine.h"
#include "ResourceManager.hpp"
#include "Mesh.hpp"

#include <iostream>

NXS_NAMESPACE {
    class Data;
    
    class MeshResourceManager : public ResourceManager<Mesh>
    {
    public:
    protected:
        /**
         * Create a resource from the specified @c path .
         */
        Ref<Mesh> create(const std::string& path) override;
    };
}

#endif /* __NXS_MESH_RESOURCE_MANAGER_H__ */
