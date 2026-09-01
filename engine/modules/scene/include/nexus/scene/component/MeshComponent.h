#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ecs/Component.h"
#include "nexus/graphics/Mesh.h"

NXS_NAMESPACE
{
    class MeshComponent : public IComponent
    {
    public:
        using Super = IComponent;

        IMPLEMENT_COMPONENT(MeshComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Mesh");
            reflector.VisitBool("Show Bounding Sphere", showBoundingSphere);
            reflector.VisitBool("Show Bounding Box", showBoundingBox);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["meshPath"] = meshPath;
            data["showBoundingBox"] = showBoundingBox;
            data["showBoundingSphere"] = showBoundingSphere;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            meshPath = data["meshPath"].GetString();
            showBoundingBox = data["showBoundingBox"].GetBool();
            showBoundingSphere = data["showBoundingSphere"].GetBool();
            return true;
        }

        void Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface) override
        {
            // TODO: Resolve mesh
        }

        void SetMesh(Ref<Mesh> mesh)
        {
            m_mesh = mesh;
        }

        Ref<Mesh> GetMesh() const
        {
            return m_mesh;
        }

        std::string meshPath;
        bool visible = true;
        bool showBoundingBox = false;
        bool showBoundingSphere = false;

    private:
        Ref<Mesh> m_mesh;
    };

    class PlaneMeshComponent : public MeshComponent
    {
    public:
        using Super = MeshComponent;

        IMPLEMENT_COMPONENT(PlaneMeshComponent);

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["width"] = width;
            data["height"] = height;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            width = data["width"].GetFloat();
            height = data["height"].GetFloat();
            return true;
        }

        void Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface) override
        {
            auto mesh = PrimitiveMesh::CreatePlane(
                meshPath,
                width, height,
                renderingInterface);
            SetMesh(mesh);
        }

        float width, height;
    };

    class BoxMeshComponent : public MeshComponent
    {
    public:
        using Super = MeshComponent;

        IMPLEMENT_COMPONENT(BoxMeshComponent);
    };

}