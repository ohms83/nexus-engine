//
// Created by nutta on 7/10/2025.
//

#pragma once

#include "NxsGL.h"
#include "../TextureProxy.h"

#include <atomic>

NXS_NAMESPACE
{
    class GLTexture final : public TextureProxy
    {
    public:
        ~GLTexture() override;

        void Bind() const override;
        void Unbind() const override;
        bool IsBinding() const override;

        TextureProxy& Begin(const TextureDescription& info) override;
        TextureProxy& LoadData(const uint8* data, uint32 size) override;
        TextureProxy& LoadMipData(const uint8* data, uint32 size, uint32 mip) override;

        void CopyData(const void* data, size_t bytes, size_t offset = 0) override;

    protected:
        NODISCARD uint32 Alloc() override;
        void Release() override;

    private:
        static std::atomic<uint32> s_bindingTexture;
    };
}
