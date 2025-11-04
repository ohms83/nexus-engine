#include "nexus/graphics/Model.h"
#include "nexus/core/LogDispatcher.h"

#include "Remotery.h"

#include <sstream>

USING_NAMESPACE_NXS;

DEFINE_LOG(Model);

void Model::AddMesh(const Ref<Mesh>& mesh)
{
    m_meshes.push_back(mesh);
}

void Model::SetBoundingBox(const glm::vec3& position, const glm::vec3& boxExt)
{
    m_box = Box {position, boxExt};
}

void Model::SetBoundingSphere(const glm::vec3& center, float radius)
{
    m_sphere = Sphere {center, radius};
}

std::string Model::DumpStats() const
{
    std::stringstream output;
    output << std::format("Model: Path={} Num Mesh={}\n", GetPath(), m_meshes.size());

    for (const auto mesh : m_meshes)
    {
        output << std::format("  Mesh: Name={} Vertex={} Polygons={} Texture Count={}\n",
            mesh->GetName(),
            mesh->GetVertexBuffer()->VertexCount(),
            mesh->GetIndexBuffer()->NumPolygons(),
            mesh->GetMaterial()->TextureCount());
    }

    return output.str();
}