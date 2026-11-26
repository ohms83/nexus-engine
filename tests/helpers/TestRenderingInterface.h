#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/RenderTarget.h"

USING_NAMESPACE_NXS;

class FakdeVertexBuffer : public VertexBuffer
{
public:
    using Super = VertexBuffer;

    FakdeVertexBuffer() {}
    ~FakdeVertexBuffer() override {}

    NODISCARD uint32_t GetHandle() const override { return 1; }

    void Bind() const override {}
    void Unbind() const override {}
    bool IsBinding() const override { return false; }

    void CopyData(const void* data, size_t bytes, size_t offset) override {}

protected:
    void Build_Impl() override {}

    NODISCARD uint32_t Alloc() override { return 1; }
    void Release() override {}
};

class FakeIndexBuffer : public IndexBuffer
{
public:
    using Super = IndexBuffer;

    FakeIndexBuffer() {}
    ~FakeIndexBuffer() override {}

    NODISCARD uint32_t GetHandle() const override { return 1; }

    void Bind() const override {}
    void Unbind() const override {}
    bool IsBinding() const override { return false; }

protected:
    void Build_Impl() override {}

    NODISCARD uint32_t Alloc() override { return 1; }
    void Release() override {}
};

class FakeTexture : public TextureProxy
{
public:
    using Base = TextureProxy;

    FakeTexture() : TextureProxy(TextureDescription {}) { m_textureID = 1; }
    ~FakeTexture() override {}

    NODISCARD uint32_t GetHandle() const override { return 1; }

    void Bind() const override {}
    void Unbind() const override {}
    bool IsBinding() const override { return false; }

    void LoadData(const uint8_t* data, uint32_t size) override {}
    void LoadData(TextureTarget target, const uint8_t* data, uint32_t size) override {}
    void LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip) override {}

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

class FakeRenderTarget : public RenderTarget
{
public:
    void Bind(RenderSystem&) override {}
    void Unbind(RenderSystem&) override {}
    NODISCARD uint32_t GetWidth() const override { return 0; }
    NODISCARD uint32_t GetHeight() const override { return 0; }
    NODISCARD Ref<TextureProxy> GetDepthTextureProxy() const override
    {
        return std::make_shared<FakeTexture>();
    }
};

class FakeGpuResourceFactory : public IGpuResourceFactory
{
public:
    FakeGpuResourceFactory() {}
    ~FakeGpuResourceFactory() override {}

    Ref<GpuProgram> CreateGpuProgram() const override { return std::make_shared<FakeGpuProgram>(); }
    Ref<TextureProxy> CreateTextureProxy(const TextureDescription&) const override { return std::make_shared<FakeTexture>(); }
    Ref<TextureProxy> CreateCubeTexture(int32_t, int32_t) const override { return nullptr; }
    Ref<IndexBuffer> CreateIndexBuffer() const override { return std::make_shared<FakeIndexBuffer>(); }
    Ref<VertexBuffer> CreateVertexBuffer() const override { return std::make_shared<FakdeVertexBuffer>(); }
    Ref<RenderTarget> CreateRenderTarget(const TextureDescription&) const override
    {
        return std::make_shared<FakeRenderTarget>();
    }
};

class FakeRendering : public RenderingInterface, public IGpuResourceFactory
{
public:
    FakeRendering() {}
    ~FakeRendering() override {}

    void DebugMarker(const std::string& message) override {}

    Ref<GpuProgram> CreateGpuProgram() const override { return std::make_shared<FakeGpuProgram>(); }
    Ref<TextureProxy> CreateTextureProxy(const TextureDescription&) const override { return std::make_shared<FakeTexture>(); }
    Ref<TextureProxy> CreateCubeTexture(int32_t, int32_t) const override { return nullptr; }
    Ref<IndexBuffer> CreateIndexBuffer() const override { return std::make_shared<FakeIndexBuffer>(); }
    Ref<VertexBuffer> CreateVertexBuffer() const override { return std::make_shared<FakdeVertexBuffer>(); }
    Ref<RenderTarget> CreateRenderTarget(const TextureDescription&) const override
    {
        return std::make_shared<FakeRenderTarget>();
    }
};

class BadGpuProgram : public GpuProgram
{
private:
    uint32_t Alloc() override { return 0; }
};

class BadRendering : public FakeRendering
{
public:
    Ref<GpuProgram> CreateGpuProgram() const override { return std::make_shared<BadGpuProgram>(); }
};

class FakeRenderer final : public SceneRenderer
{
public:
    FakeRenderer() = default;
    ~FakeRenderer() override = default;

    void Render(RenderSystem& renderSystem, const Scene& scene) override {}
};
