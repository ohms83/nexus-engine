#include "nexus/graphics/RHIPlugin.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/RenderingInterface.h"
#include "nexus/GLRenderingInterface.h"
#include "nexus/GLVertexBuffer.h"
#include "nexus/GLIndexBuffer.h"
#include "nexus/GLGpuProgram.h"
#include "nexus/GLTexture.h"
#include "nexus/GLRenderTarget.h"

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

        NODISCARD Ref<RenderTarget> CreateRenderTarget(
            const uint32_t width,
            const uint32_t height,
            const bool depthOnly) const override
        {
            return NewRef<GLRenderTarget>(
                width,
                height,
                depthOnly ? nullptr : CreateTextureProxy(),
                CreateTextureProxy(),
                depthOnly ? PixelFormat::Depth : PixelFormat::RGBA,
                depthOnly);
        }
    };

    class GLRHIPlugin : public IRHIPlugin
    {
    public:
        GLRHIPlugin() = default;
        virtual ~GLRHIPlugin() = default;

        IGpuResourceFactory* GetGpuResourceFactory() override
        {
            return &m_factory;
        }

        Ref<RenderingInterface> CreateRenderingInterface(
            const WindowContext window,
            const GraphicsConfig& config) override
        {
            return NewRef<GLRenderingInterface>(window, config);
        }

    private:
        GLResourceFactory m_factory;
    };

    static GLRHIPlugin* s_plugin = nullptr;
    
    IRHIPlugin* InitRHI(const GraphicsConfig&)
    {
        s_plugin = new GLRHIPlugin();
        return s_plugin;
    }

    IRHIPlugin* GetRHI()
    {
        return s_plugin;
    }

    void DestroyRHI()
    {
        if (s_plugin)
        {
            delete s_plugin;
            s_plugin = nullptr;
        }
    }
}