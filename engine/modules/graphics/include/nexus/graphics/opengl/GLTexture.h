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
        NODISCARD bool IsBinding() const override;

        TextureProxy& Begin(const TextureDescription& info) override;
        TextureProxy& LoadData(const uint8_t* data, uint32_t size) override;
        TextureProxy& LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip) override;

        void CopyData(const void* data, size_t bytes, size_t) override;

    protected:
        NODISCARD uint32_t Alloc() override;
        void Release() override;

    private:
        static std::atomic<uint32_t> s_bindingTexture;
    };
}
