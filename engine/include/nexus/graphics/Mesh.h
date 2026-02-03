#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/serialize/Serializeable.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "nexus/geom/Sphere.h"
#include "nexus/geom/Box.h"

NXS_NAMESPACE
{
    class MaterialManager;
    class TextureManager;
    class Mesh : public ISerializeable
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
        MAYBE_UNUSED bool Deserialize(const VariantData& data) override;

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

        void Resolve(class ResourceManager& resourceManager);

    private:
        virtual VariantData SerializeVertices() const;
        virtual VariantData SerializeIndices() const;

        virtual bool DeserializeVertices();
        virtual bool DeserializeIndices();

    protected:
        // Mesh stores the material path when deserializing and will attempt to resolve it when Resolve() is called.
        std::string m_materialPath;
        std::string m_name;
        std::vector<float> m_vertices;
        std::vector<uint32> m_indices;
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
            const RenderingInterface& renderingInterface,
            Ref<Material> material = nullptr
        );

        static Ref<Mesh> CreateBox(
            std::string name,
            const glm::vec3& size,
            const RenderingInterface& renderingInterface,
            Ref<Material> material = nullptr
        );

        static Ref<Mesh> CreateSphere(
            std::string name,
            float radius,
            uint32_t sectorCount,
            uint32_t stackCount,
            const RenderingInterface& renderingInterface,
            Ref<Material> material = nullptr
        );
    };
}