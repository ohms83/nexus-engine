#include <nexus/resource/Mesh.h>

#include "resource/mesh/CubeMesh.h"
#include "resource/mesh/PlaneMesh.h"

USING_NAMESPACE_NXS;

const std::string Mesh::CubeMesh = "CubeMesh";
const std::string Mesh::PlaneMesh = "PaneMesh";

Mesh::Mesh(const uint32 hash)
    : Resource(hash)
{
}

uint8* Mesh::Load_Impl(const std::string& path, size_t& out_size)
{
    return nullptr;
}

MeshManager::MeshManager()
{
    m_cubeMesh = std::make_shared<CubeMesh>(m_hasher.Hash32(Mesh::CubeMesh));
    m_planeMesh = std::make_shared<PlaneMesh>(m_hasher.Hash32(Mesh::PlaneMesh));
}

MeshManager::~MeshManager()
{
    NXS_ASSERT_MSG(m_planeMesh.use_count() <= 1, "Plane mesh wasn't released properly. Please make sure to release all the shared resources before destroying the manager");
    NXS_ASSERT_MSG(m_cubeMesh.use_count() <= 1, "Cube mesh wasn't released properly. Please make sure to release all the shared resources before destroying the manager");
}

Ref<Mesh> MeshManager::GetStaticMesh(const std::string& meshName)
{
    if (meshName == Mesh::CubeMesh) {
        return m_cubeMesh;
    }
    if (meshName == Mesh::PlaneMesh) {
        return m_planeMesh;
    }
    
    return nullptr;
}
