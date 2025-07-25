#pragma once

#include <nexus/NxsDefine.h>
#include <nexus/graphics/VertexBuffer.h>
#include <nexus/graphics/IndexBuffer.h>
#include <nexus/graphics/Shader.h>

#include "Resource.h"
#include "ResourceManager.h"

NXS_NAMESPACE
{
    class Mesh : public Resource
    {
    public:
        explicit Mesh(uint32 resourceId);
        ~Mesh() override = default;

        NODISCARD VertexBuffer* GetVertexBuffer() const
        {
            return m_vertexBuffer.get();
        }

        NODISCARD IndexBuffer* GetIndexBuffer() const
        {
            return m_indexBuffer.get();
        }

        void SetShader(Shader* shader)
        {
            m_shader = shader;
        }
        NODISCARD Shader* GetShader() const
        {
            return m_shader;
        }

        static const std::string CubeMesh;
        static const std::string PlaneMesh;

    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override;
        
    protected:
        Ptr<VertexBuffer> m_vertexBuffer;
        Ptr<IndexBuffer> m_indexBuffer;
        Shader* m_shader = nullptr;
    };

    class MeshManager final : public ResourceManager<Mesh>
    {
    public:
        MeshManager();
        ~MeshManager() override;

        Ref<Mesh> GetStaticMesh(const std::string& meshName);

    private:
        Ref<Mesh> m_cubeMesh;
        Ref<Mesh> m_planeMesh;
    };
}