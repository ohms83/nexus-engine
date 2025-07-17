#include <nexus/resource/Mesh.h>

USING_NAMESPACE_NXS;

const std::string Mesh::CubeMesh = "CubeMesh";

uint8* Mesh::Load_Impl(const std::string& path, size_t& out_size)
{
    return nullptr;
}

MeshManager::MeshManager()
{
    m_cubeMesh = std::make_shared<Mesh>(m_hasher.Hash32(Mesh::CubeMesh));
}

Ref<Mesh> MeshManager::GetStaticMesh(const std::string& meshName)
{
    if (meshName == Mesh::CubeMesh) {
        return m_cubeMesh;
    }
    
    return nullptr;
}