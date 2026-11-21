#pragma once

#include "nexus/graphics/CubeTexture.h"
#include <atomic>

NXS_NAMESPACE
{
    class GLCubeTexture final : public CubeTexture
    {
    public:
        explicit GLCubeTexture(int32_t width, int32_t height);
        virtual ~GLCubeTexture();

        MAYBE_UNUSED uint32_t Alloc() override;

        //! Release the allocated GPU resource.
        void Release() override;

        NODISCARD uint32_t GetHandle() const override { return m_textureID; }

        void Bind() const override;
        void Unbind() const override;
        NODISCARD bool IsBinding() const override;

        void CopyData(const void* data, size_t bytes, size_t offset) override {}

        // void LoadData(Face face, Ref<IBuffer> data) override;
        // void LoadData(Array<Ref<IBuffer>> data) override;
        void LoadData(Face face, const uint8_t* data, uint32_t size) override;

    private:
        static std::atomic<uint32_t> s_bindingTexture;
    };
}