#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderSystem.h"

USING_NAMESPACE_NXS;

class FakeGpuProgram final : public GpuProgram
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
    NODISCARD uint32_t Alloc() { return 1; }
    void Release() override {}

    int32_t FindUniform_Internal(const std::string& name) const { return 0; }
};

class FakeRendering final : public RenderingInterface
{
public:
    FakeRendering() {}
    ~FakeRendering() override {}

    void ClearColor(const Color4F& color) override { }
    void ClearDepth(float depth) override { }
    void ClearBuffer(const Color4F& color, float depth) override { }
    void SwapBuffer() override { }
    void SetViewport(int32 x, int32 y, int32 w, int32 h) override { }
    VertexBuffer* CreateVertexBuffer() const override { return nullptr; }
    IndexBuffer* CreateIndexBuffer() const override { return nullptr; }
    GpuProgram* CreateGpuProgram() const override { return new FakeGpuProgram(); }
    TextureProxy* CreateTexture() const override { return nullptr; }
    void OnResize(uint32_t pixel_w, uint32_t pixel_h) override { }
    void DrawIndexed(const Ref<IndexBuffer> indexBuffer) override { }
    void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32 instanceCount) override { }
    void SetColorMask(const glm::bvec4& mask) override { }
    void SetDepthMask(bool mask) override { }
    void SetDepthFunction(DepthFunction depthFunction) override { }
    void SetLineWidth(float width) override { }
    void EnableDrawBuffer(DrawBuffer buffer) override { }
};

class BadGpuProgram : public GpuProgram
{
public:
    using Super = GpuProgram;

    GpuProgram& BeginCompile() override { Super::BeginCompile(); return *this; }
    GpuProgram& AddSource(const std::string& source, Type shaderType) override { Super::AddSource(source, shaderType); return *this; }
    void Compile() override { Super::Compile(); }
    void Bind() override {}
    void Unbind() override {}
    bool IsBinding() const override { return false; }
    bool SetUniformInt(const std::string&, int32_t) override { return true; }
    bool SetUniformFloat(const std::string&, float) override { return true; }
    bool SetUniformVector(const std::string&, const glm::vec2&) override { return true; }
    bool SetUniformVector(const std::string&, const glm::vec3&) override { return true; }
    bool SetUniformVector(const std::string&, const glm::vec4&) override { return true; }
    bool SetUniformMatrix(const std::string&, const glm::mat3&, bool) override { return true; }
    bool SetUniformMatrix(const std::string&, const glm::mat4&, bool) override { return true; }
    bool SetUniformTexture2D(const std::string&, Ref<const TextureProxy>, int32_t) override { return true; }
private:
    uint32_t Alloc() override { return 0; }
    void Release() override {}
    int32_t FindUniform_Internal(const std::string&) const override { return 0; }
};

class BadRendering : public RenderingInterface
{
public:
    GpuProgram* CreateGpuProgram() const override { return new BadGpuProgram(); }
    void ClearColor(const Color4F&) override {}
    void ClearDepth(float) override {}
    void ClearBuffer(const Color4F&, float) override {}
    void SwapBuffer() override {}
    void SetViewport(int32 x, int32 y, int32 w, int32 h) override {}
    VertexBuffer* CreateVertexBuffer() const override { return nullptr; }
    IndexBuffer* CreateIndexBuffer() const override { return nullptr; }
    TextureProxy* CreateTexture() const override { return nullptr; }
    void OnResize(uint32_t pixel_w, uint32_t pixel_h) override {}
    void DrawIndexed(const Ref<IndexBuffer> indexBuffer) override {}
    void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32 instanceCount) override {}
    void SetColorMask(const glm::bvec4& mask) override {}
    void SetDepthMask(bool mask) override {}
    void SetDepthFunction(DepthFunction depthFunction) override {}
    void SetLineWidth(float width) override {}
    void EnableDrawBuffer(DrawBuffer buffer) override {}
};

class FakeRenderer final : public SceneRenderer
{
public:
    FakeRenderer() = default;
    ~FakeRenderer() override = default;

    void Render(RenderSystem& renderSystem, const entt::registry& registry) override {}
};
