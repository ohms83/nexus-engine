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

std::vector<RenderCommand> Model::CreateDrawCommand() const
{
    rmt_ScopedCPUSample(Model_CreateDrawCommand, 0);
    std::vector<RenderCommand> commands;

    for (const auto mesh : m_meshes)
    {
        rmt_ScopedCPUSample(Mesh_DrawCommand, 0);
        const auto material = mesh->GetMaterial();
        RenderCommand command = {
            material->GetShader()->GetGpuProgram(),
            mesh->GetVertexBuffer(),
            mesh->GetIndexBuffer(),
        };
        auto& sphere = mesh->GetSphere();
        command.sphereRadius = sphere.radius;
        material->WriteRenderCommand(command);

        {
            rmt_ScopedCPUSample(CommandList_Emplace, 0);
            commands.emplace_back(std::move(command));
        }
    }
    return commands;
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