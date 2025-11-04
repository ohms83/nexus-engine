#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/core/Resource.h"
#include "nexus/geom/Box.h"
#include "nexus/geom/Sphere.h"
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

        void SetBoundingBox(const glm::vec3& position, const glm::vec3& boxExt);

        const Box& GetBoundingBox() const
        {
            return m_box;
        }

        void SetBoundingSphere(const glm::vec3& center, float radius);

        const Sphere& GetBoundingSphere() const
        {
            return m_sphere;
        }

        std::string DumpStats() const;

    private:
        std::vector<Ref<Mesh>> m_meshes;
        //! The smallest bounding sphere that can cover the entire model.
        Sphere m_sphere {};
        //! The smallest bounding-box's that can cover the entire model.
        Box m_box;
    };
}