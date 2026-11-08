#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderSystem.h"

USING_NAMESPACE_NXS;

class FakeTexture : public TextureProxy
{
public:
    using Super = TextureProxy;

    FakeTexture() {}
    ~FakeTexture() override {}

    NODISCARD uint32_t GetHandle() const override { return 1; }

    void Bind() const override {}
    void Unbind() const override {}
    bool IsBinding() const override { return false; }

    TextureProxy& Begin(const TextureDescription& info) override
    { 
        m_textureID = 1;
        return *this;
    }

    TextureProxy& LoadData(const uint8_t* data, uint32_t size) override { return *this; }
    TextureProxy& LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip) override { return *this; }

    void CopyData(const void* data, size_t bytes, size_t offset) override {}

protected:
    NODISCARD uint32_t Alloc() override { return 1; }
    void Release() override {}
};

class FakeGpuProgram : public GpuProgram
{
private:
    NODISCARD uint32_t Alloc() override { return 1; }
};

class FakeRendering : public RenderingInterface
{
public:
    FakeRendering() {}
    ~FakeRendering() override {}

    GpuProgram* CreateGpuProgram() const override { return new FakeGpuProgram(); }
    TextureProxy* CreateTexture() const override { return new FakeTexture(); }
};

class BadGpuProgram : public GpuProgram
{
private:
    uint32_t Alloc() override { return 0; }
};

class BadRendering : public FakeRendering
{
public:
    GpuProgram* CreateGpuProgram() const override { return new BadGpuProgram(); }
};

class FakeRenderer final : public SceneRenderer
{
public:
    FakeRenderer() = default;
    ~FakeRenderer() override = default;

    void Render(RenderSystem& renderSystem, const Scene& scene) override {}
};
