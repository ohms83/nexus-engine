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

        static const std::string CubeMesh;
        static const std::string PlaneMesh;

    protected:
        std::string m_name;
        Ref<VertexBuffer> m_vertexBuffer;
        Ref<IndexBuffer> m_indexBuffer;
        Ref<Material> m_material;
    };
}