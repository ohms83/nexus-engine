#include "nexus/graphics/Model.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Model);

void Model::AddMesh(const Ref<Mesh>& mesh)
{
    m_meshes.push_back(mesh);
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
