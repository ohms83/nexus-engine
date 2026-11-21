//
// Created by nutta on 7/10/2025.
//

#pragma once

#include "NxsGL.h"
#include "nexus/graphics/TextureProxy.h"

#include <atomic>

NXS_NAMESPACE
{
    namespace GL
    {
        inline GLuint NxsTextureWrapModeToGL(const TextureWrapMode mode)
        {
            switch (mode)
            {
            case TextureWrapMode::Clamp:
                return GL_CLAMP;
            case TextureWrapMode::Repeat:
                return GL_REPEAT;
            case TextureWrapMode::MirroredRepeat:
                return GL_MIRRORED_REPEAT;
            case TextureWrapMode::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            case TextureWrapMode::ClampToBorder:
                return GL_CLAMP_TO_BORDER;
            case TextureWrapMode::MirroredClampToEdge:
                return GL_MIRROR_CLAMP_TO_EDGE;
            default:
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }

        inline GLuint NxsTextureFilterModeToGL(const TextureFilterMode mode)
        {
            switch (mode)
            {
            case TextureFilterMode::Nearest:
                return GL_NEAREST;
            case TextureFilterMode::Linear:
                return GL_LINEAR;
            case TextureFilterMode::NearestMipmapNearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case TextureFilterMode::LinearMipmapNearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case TextureFilterMode::NearestMipmapLinear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case TextureFilterMode::LinearMipmapLinear:
                return GL_LINEAR_MIPMAP_LINEAR;
            default:
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }

        inline GLint NxsPixelFormatToGL(const PixelFormat format)
        {
            switch (format)
            {
            case PixelFormat::None:
                break;
            case PixelFormat::Grey:
            case PixelFormat::Red:
                // OpenGL has no explicit Greyscale format. The greyscale textures are normally
                // represented by a single channel red component.
                return GL_RED;
            case PixelFormat::Green:
                return GL_GREEN;
            case PixelFormat::Blue:
                return GL_BLUE;
            case PixelFormat::Alpha:
                return GL_RGBA;
            case PixelFormat::RedGreen:
                return GL_RG;
            case PixelFormat::RGB:
                return GL_RGB;
            case PixelFormat::RGBA:
                return GL_RGBA;
            case PixelFormat::Depth:
                return GL_DEPTH_COMPONENT;
            case PixelFormat::Stencil:
                return GL_STENCIL_INDEX;
            default:
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }
    }

    class GLTexture final : public TextureProxy
    {
    public:
        explicit GLTexture(const TextureDescription& desc);
        ~GLTexture() override;

        MAYBE_UNUSED uint32_t Alloc() override;
        void Release() override;

        void Bind() const override;
        void Unbind() const override;
        NODISCARD bool IsBinding() const override;

        void LoadData(const uint8_t* data, uint32_t size) override;
        void LoadData(TextureTarget target, const uint8_t* data, uint32_t size) override;
        void LoadData(Face face, const uint8_t* data, uint32_t size) override;
        void LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip) override;

        void CopyData(const void* data, size_t bytes, size_t) override;
    };
}
