#pragma once

#include "nexus/core/ResourceLoader.h"
#include "RenderingInterface.h"
#include "Texture.h"

NXS_NAMESPACE
{
    /**
     * @brief Implements IResourceLoader to load Texture resources.
     * Encapsulates file reading and GPU texture creation.
     */
    class TextureLoader final : public IResourceLoader
    {
    public:
        explicit TextureLoader(const Ref<RenderingInterface>& renderingInterface);

        Ref<Resource> Load(const std::string& path, uint32 id) override;
        void LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback) override;

    protected:
        class TextureBuffer : public IBuffer
        {
        public:
            bool IsValid() const override
            {
                return pixels != nullptr;
            }

            NODISCARD const uint8* Data() const override
            {
                return pixels.get();
            }

            NODISCARD uint8* Data() override
            {
                return pixels.get();
            }

            NODISCARD uint64 Size() const override
            {
                return size;
            }

            void Release() override
            {
                pixels.reset();
                size = 0;
                desc = TextureDescription {};
            }

            uint64_t CopyData(uint8_t* data, uint64_t bytes, uint64_t offset = 0) override
            {
                return 0;
            }

            Ptr<uint8_t[]> pixels;
            uint64_t size = 0;
            TextureDescription desc {};
        };
        Ref<IBuffer> PerformLoadFile(const std::string& path) override;

    private:
        Ref<RenderingInterface> m_renderingInterface;
    };
}