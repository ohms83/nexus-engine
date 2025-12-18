#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/io/Serializable.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "nexus/geom/Sphere.h"
#include "nexus/geom/Box.h"

NXS_NAMESPACE
{
    class MaterialManager;
    class TextureManager;
    class Mesh : public ISerializable
    {
    public:
        Mesh();
        explicit Mesh(std::string name);
        virtual ~Mesh() = default;

        NODISCARD const std::string& GetName() const { return m_name; }

        void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
        {
            m_vertexBuffer = vertexBuffer;
        }

        NODISCARD Ref<VertexBuffer> GetVertexBuffer() const
        {
            return m_vertexBuffer;
        }

        void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
        {
            m_indexBuffer = indexBuffer;
        }

        NODISCARD Ref<IndexBuffer> GetIndexBuffer() const
        {
            return m_indexBuffer;
        }

        void SetMaterial(const Ref<Material>& material)
        {
            m_material = material;
        }

        NODISCARD Ref<Material> GetMaterial() const
        {
            return m_material;
        }

        // Serialization
        VariantData Serialize() const override;
        void Deserialize(const VariantData& data) override;

        void SetSphere(const Sphere& sphere)
        {
            m_boundingSphere = sphere;
        }

        NODISCARD const Sphere& GetSphere() const
        {
            return m_boundingSphere;
        }

        void SetBox(const Box& box)
        {
            m_boundingBox = box;
        }

        NODISCARD const Box& GetBox() const
        {
            return m_boundingBox;
        }

        void ComputeBounds();

    public:
        // Resolve a material reference using a MaterialManager and TextureManager. This will set m_material
        // if the mesh currently only stores a material path (e.g., deserialized state). It will also cause
        // the material to resolve its textures via the texture manager.
        void Resolve(class MaterialManager& materialManager, class TextureManager& textureManager);

        // Mesh stores the material path when deserializing and will attempt to resolve it when Resolve() is called.
        std::string m_materialPath;
        std::string m_name;
        Ref<VertexBuffer> m_vertexBuffer;
        Ref<IndexBuffer> m_indexBuffer;
        Ref<Material> m_material;
        //! @brief Bounding sphere.
        Sphere m_boundingSphere {};
        Box m_boundingBox {};
    };

    class PrimitiveMesh
    {
    public:
        static Ref<Mesh> CreatePlane(
            std::string name,
            float width, float height,
            Ref<RenderingInterface> renderingInterface,
            Ref<Material> material = nullptr
        );

        static Ref<Mesh> CreateBox(
            std::string name,
            const glm::vec3& size,
            Ref<RenderingInterface> renderingInterface,
            Ref<Material> material = nullptr
        );

        static Ref<Mesh> CreateSphere(
            std::string name,
            float radius,
            uint32_t sectorCount,
            uint32_t stackCount,
            Ref<RenderingInterface> renderingInterface,
            Ref<Material> material = nullptr
        );
    };
}