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
void Model::ComputeBounds()
{
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };
    for (const auto& mesh : m_meshes)
    {
        mesh->ComputeBounds();
        const auto& box = mesh->GetBox();
        min = glm::min(min, box.GetMin());
        max = glm::max(max, box.GetMax());
    }

    glm::vec3 sphereCenter = (max + min) * 0.5f;
    glm::vec3 extent = max - sphereCenter;
    float sphereRadius = glm::length(extent);
    SetBoundingBox(sphereCenter, extent);
    SetBoundingSphere(sphereCenter, sphereRadius);
}