//
// Created by nutta on 7/10/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/TextureProxy.h>

NXS_NAMESPACE
{
    class GLTexture final : public TextureProxy
    {
    public:
        ~GLTexture() override;

        void Bind() const override;
        void Unbind() const override;

        TextureProxy& Begin(const TextureCreationInfo& info) override;
        TextureProxy& LoadData(const uint8* data, uint32 size) override;
        TextureProxy& LoadMipData(const uint8* data, uint32 size, uint32 mip) override;

    protected:
        NODISCARD uint32 Alloc() override;
        void Release() override;
    };
}
