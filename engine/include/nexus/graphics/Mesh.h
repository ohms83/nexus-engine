#pragma once

#include "nexus/NxsDefine.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "nexus/geom/Sphere.h"
#include "nexus/geom/Box.h"

NXS_NAMESPACE
{
    class Mesh
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

    protected:
        std::string m_name;
        Ref<VertexBuffer> m_vertexBuffer;
        Ref<IndexBuffer> m_indexBuffer;
        Ref<Material> m_material;
        //! @brief Bounding sphere.
        Sphere m_boundingSphere {};
        Box m_boundingBox {};
    };
}