#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Resource.h"
#include "Mesh.h"

#include <vector>

#include "RenderSystem.h"

struct aiMaterial;
struct aiMesh;
struct aiScene;
struct aiNode;

NXS_NAMESPACE
{
    class Model final : public Resource
    {
    public:
        explicit Model(std::string path, const uint32 resourceId)
            : Resource(std::move(path), resourceId)
        {
        }

        ~Model() override = default;

        void AddMesh(const Ref<Mesh>& mesh);

        const std::vector<Ref<Mesh>>& GetMeshes() const
        {
            return m_meshes;
        }

        std::vector<RenderCommand> CreateDrawCommand() const;

    private:
        std::vector<Ref<Mesh>> m_meshes;
        //! A radius of the smallest bounding sphere that can cover the entire model.
        float m_sphereRadius = 0;
        //! The smallest value of the bounding-box's dimention in x/y/z coordinate that can cover the entire model.
        glm::vec3 m_boxExtent{};
    };
}