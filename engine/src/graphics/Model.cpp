#include "nexus/graphics/Model.h"
#include "nexus/core/LogDispatcher.h"

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
    std::vector<RenderCommand> commands;
    for (const auto mesh : m_meshes)
    {
        const auto material = mesh->GetMaterial();
        RenderCommand command = {
            material->GetShader()->GetGpuProgram(),
            mesh->GetVertexBuffer(),
            mesh->GetIndexBuffer(),
        };
        material->WriteRenderCommand(command);

        commands.emplace_back(std::move(command));
    }
    return commands;
}
