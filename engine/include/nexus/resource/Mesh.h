#include <nexus/NxsDefine.h>
#include <bgfx/bgfx.h>

NXS_NAMESPACE
{
    class Mesh
    {
    public:
        
    protected:
        bgfx::VertexBufferHandle m_vertexBuffer{};
        bgfx::IndexBufferHandle m_indexBuffer{};
        bgfx::ProgramHandle m_shader{};
    };
}