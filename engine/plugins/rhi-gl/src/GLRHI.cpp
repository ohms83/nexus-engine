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

        NODISCARD virtual Ref<TextureProxy> CreateTextureProxy(const TextureDescription& desc) const override
        {
            auto texture = NewRef<GLTexture>(desc);
            texture->Alloc();
            texture->Bind();

            GLuint gl_wrapModeS = GL::NxsTextureWrapModeToGL(desc.wrapModeS);
            GLuint gl_wrapModeT = GL::NxsTextureWrapModeToGL(desc.wrapModeT);
            GLuint gl_wrapModeR = GL::NxsTextureWrapModeToGL(desc.wrapModeR);

            GLuint gl_filterMin = GL::NxsTextureFilterModeToGL(desc.filterMin);
            GLuint gl_filterMag = GL::NxsTextureFilterModeToGL(desc.filterMag);

            const GLenum gl_target = desc.kind == TextureKind::TextureCube
                ? GL_TEXTURE_CUBE_MAP
                : GL_TEXTURE_2D;
            CALL_GL_FUNC(glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, gl_wrapModeS));
            CALL_GL_FUNC(glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, gl_wrapModeT));
            if (desc.kind == TextureKind::TextureCube)
            {
                CALL_GL_FUNC(glTexParameteri(gl_target, GL_TEXTURE_WRAP_R, gl_wrapModeR));
            }
            CALL_GL_FUNC(glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, gl_filterMag));
            CALL_GL_FUNC(glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, gl_filterMin));
            texture->Unbind();
            return texture;
        }

        NODISCARD virtual Ref<TextureProxy> CreateCubeTexture(int32_t width, int32_t height) const override
        {
            TextureDescription desc {
                .width = width,
                .height = height,
                .kind = TextureKind::TextureCube,
                .channels = 3,
                .format = PixelFormat::RGB,
                .componentType = DataType::UByte,
                .colorSpace = TextureColorSpace::SRgb,
                .wrapModeS = TextureWrapMode::ClampToEdge,
                .wrapModeT = TextureWrapMode::ClampToEdge,
                .wrapModeR = TextureWrapMode::ClampToEdge,
                .filterMin = TextureFilterMode::Linear,
                .filterMag = TextureFilterMode::Linear,
                .numMips = 1,
            };
            return CreateTextureProxy(desc);
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
            TextureDescription colorDesc;
            colorDesc.width = INT_CAST(width);
            colorDesc.height = INT_CAST(height);
            colorDesc.format = depthOnly ? PixelFormat::Depth : PixelFormat::RGBA;

            TextureDescription depthDesc;
            depthDesc.width = INT_CAST(width);
            depthDesc.height = INT_CAST(height);
            depthDesc.channels = 1;
            depthDesc.format = PixelFormat::Depth;
            depthDesc.filterMin = TextureFilterMode::Nearest;
            depthDesc.filterMag = TextureFilterMode::Nearest;
            depthDesc.wrapModeS = TextureWrapMode::ClampToEdge;
            depthDesc.wrapModeT = TextureWrapMode::ClampToEdge;

            auto colorTexture = depthOnly ? nullptr : CreateTextureProxy(colorDesc);
            auto depthTexture = CreateTextureProxy(depthDesc);
            return NewRef<GLRenderTarget>(
                width,
                height,
                colorTexture,
                depthTexture,
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