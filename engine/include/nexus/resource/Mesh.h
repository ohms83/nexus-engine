#pragma once

#include <nexus/NxsDefine.h>
#include <nexus/graphics/VertexBuffer.h>
#include <nexus/graphics/IndexBuffer.h>
#include <nexus/graphics/Shader.h>

#include "Resource.h"
#include "ResourceManager.h"

NXS_NAMESPACE
{
    class Mesh
    {
    public:
        Mesh();
        explicit Mesh(std::string name);
        virtual ~Mesh() = default;

        NODISCARD Ref<VertexBuffer> GetVertexBuffer() const
        {
            return m_vertexBuffer;
        }

        NODISCARD Ref<IndexBuffer> GetIndexBuffer() const
        {
            return m_indexBuffer;
        }

        static const std::string CubeMesh;
        static const std::string PlaneMesh;
        
    protected:
        std::string m_name;
        Ref<VertexBuffer> m_vertexBuffer;
        Ref<IndexBuffer> m_indexBuffer;
    };

    // class MeshManager final : public ResourceManager<Mesh>
    // {
    // public:
    //     MeshManager();
    //     ~MeshManager() override;

    //     Ref<Mesh> GetStaticMesh(const std::string& meshName);

    // private:
    //     Ref<Mesh> m_cubeMesh;
    //     Ref<Mesh> m_planeMesh;
    // };
}