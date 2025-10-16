#pragma once

#include "nexus/NxsDefine.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    class Mesh
    {
    public:
        Mesh();
        explicit Mesh(std::string name);
        virtual ~Mesh() = default;

        const std::string& GetName() const { return m_name; }

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

        void SetSpehreRadius(float r)
        {
            m_sphereRadius = r;
        }

        NODISCARD float GetSphereRadius() const
        {
            return m_sphereRadius;
        }

        void SetBoxExt(const glm::vec3& ext)
        {
            m_boxExtent = ext;
        }

        NODISCARD const glm::vec3& GetBoxExt() const
        {
            return m_boxExtent;
        }

    protected:
        std::string m_name;
        Ref<VertexBuffer> m_vertexBuffer;
        Ref<IndexBuffer> m_indexBuffer;
        Ref<Material> m_material;
        //! A radius of the smallest bounding sphere that can cover the entire mesh.
        float m_sphereRadius = 0;
        //! The smallest value of the bounding-box's dimention in x/y/z coordinate that can cover the entire mesh.
        glm::vec3 m_boxExtent{};
    };
}