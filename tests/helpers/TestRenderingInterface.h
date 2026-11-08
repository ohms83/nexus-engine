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
public:
    using Super = GpuProgram;

    FakeGpuProgram() {}
    ~FakeGpuProgram() override {}

    GpuProgram& BeginCompile() override { Super::BeginCompile(); return *this; }
    GpuProgram& AddSource(const std::string& source, Type shaderType) override { Super::AddSource(source, shaderType); return *this; }
    void Compile() override { Super::Compile(); }
    void Bind() override {}
    void Unbind() override {}
    bool IsBinding() const override { return false; }

    bool SetUniformInt(const std::string& name, int32_t value) override { return true; }
    bool SetUniformFloat(const std::string& name, float value) override { return true; }
    bool SetUniformVector(const std::string& name, const glm::vec2& vec) override { return true; }
    bool SetUniformVector(const std::string& name, const glm::vec3& vec) override { return true; }
    bool SetUniformVector(const std::string& name, const glm::vec4& vec) override { return true; }
    bool SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) override { return true; }
    bool SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) override { return true; }
    bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, int32_t textureUnit) override { return true; }

private:
    NODISCARD uint32_t Alloc() override { return 1; }
    void Release() override {}

    int32_t FindUniform_Internal(const std::string& name) const override { return 0; }
};

class FakeRendering : public RenderingInterface
{
public:
    FakeRendering() {}
    ~FakeRendering() override {}

    GpuProgram* CreateGpuProgram() const override { return new FakeGpuProgram(); }
    TextureProxy* CreateTexture() const override { return new FakeTexture(); }
};

class BadGpuProgram : public FakeGpuProgram
{
public:
    using Super = FakeGpuProgram;

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
