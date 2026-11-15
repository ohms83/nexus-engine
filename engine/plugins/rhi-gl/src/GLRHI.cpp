#include "nexus/graphics/RHIPlugin.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/opengl/GLVertexBuffer.h"
#include "nexus/graphics/opengl/GLIndexBuffer.h"
#include "nexus/graphics/opengl/GLGpuProgram.h"
#include "nexus/graphics/opengl/GLTexture.h"

NXS_NAMESPACE
{
    class GLResourceFactory : public IGpuResourceFactory
    {
    public:
        NODISCARD virtual Ref<VertexBuffer> CreateVertexBuffer() const override
        {
            return NewRef<GLVertexBuffer>();
        }

        NODISCARD virtual Ref<IndexBuffer> CreateIndexBuffer() const override
        {
            return NewRef<GLIndexBuffer>();
        }

        NODISCARD virtual Ref<TextureProxy> CreateTextureProxy() const override
        {
            return NewRef<GLTexture>();
        }

        NODISCARD virtual Ref<GpuProgram> CreateGpuProgram() const override
        {
            return NewRef<GLGpuProgram>();   
        }
    };

    class NXS_API GLRHIPlugin : public IRHIPlugin
    {
    public:
        IGpuResourceFactory* GetGpuResourceFactory() override
        {
            return &m_factory;
        }

    private:
        GLResourceFactory m_factory;
    };

    static GLRHIPlugin* s_plugin = nullptr;
    
    NXS_API_EXPORT IRHIPlugin* InitRHI(const GraphicsConfig& config)
    {
        s_plugin = new GLRHIPlugin();
        return s_plugin;
    }

    NXS_API_EXPORT IRHIPlugin* GetRHI()
    {
        return s_plugin;
    }

    NXS_API_EXPORT void DestroyRHI()
    {
        if (s_plugin)
        {
            delete s_plugin;
            s_plugin = nullptr;
        }
    }
}