#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/resource/Resource.h"
#include "nexus/geom/Box.h"
#include "nexus/geom/Sphere.h"
#include "Mesh.h"
#include "RenderSystem.h"

#include <vector>

struct aiMaterial;
struct aiMesh;
struct aiScene;
struct aiNode;

NXS_NAMESPACE
{
    class Model final : public Resource
    {
    public:
        explicit Model(std::string path, const uint32_t resourceId)
            : Resource(std::move(path), resourceId)
        {
        }

        static Ref<Model> CreateFromMesh(std::string name, Ref<Mesh> mesh)
        {
            auto model = std::make_shared<Model>(std::move(name), 0);
            model->AddMesh(mesh);
            model->ComputeBounds();
            return model;
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

        void ComputeBounds();

        std::string DumpStats() const;

    private:
        std::vector<Ref<Mesh>> m_meshes;
        //! The smallest bounding sphere that can cover the entire model.
        Sphere m_sphere {};
        //! The smallest bounding-box's that can cover the entire model.
        Box m_box;
    };
}