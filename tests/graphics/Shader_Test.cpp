#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/TestRenderingInterface.h"
#include <filesystem>
#include <fstream>

USING_NAMESPACE_NXS;

TEST(ShaderTest, SerializeDeserialize)
{
    Shader s("shaders/test", 10);
    nxs::VariantData data = nxs::VariantData::Map {
        {"vertex", std::string("void main() {}")},
        {"fragment", std::string("void main() {}")},
        {"geometry", std::string("")},
    };

    s.Deserialize(data);

    EXPECT_EQ(s.GetSource(GpuProgram::Type::Vertex), "void main() {}");
    EXPECT_EQ(s.GetSource(GpuProgram::Type::Fragment), "void main() {}");
    EXPECT_TRUE(s.HasShader(GpuProgram::Type::Vertex));
    EXPECT_TRUE(s.HasShader(GpuProgram::Type::Fragment));
    EXPECT_FALSE(s.HasShader(GpuProgram::Type::Geometry));

    auto serialized = s.Serialize();
    EXPECT_EQ(serialized["vertex"].GetString(), "void main() {}");
    EXPECT_EQ(serialized["fragment"].GetString(), "void main() {}");
}

TEST(ShaderTest, CompileExtractsUniforms)
{
    FakeRendering fake;
    Shader s("shaders/uniforms", 11);

    const std::string vert = R"(
        uniform mat4 _Model;
        uniform vec3 lightDir;
        void main(){}
    )";

    const std::string frag = R"(
        uniform vec4 color;
        uniform sampler2D albedo;
        void main(){}
    )";

    s.CompileFromSource(fake, vert, frag, "");

    EXPECT_TRUE(s.HasUniform("_Model"));
    EXPECT_TRUE(s.HasUniform("lightDir"));
    EXPECT_TRUE(s.HasUniform("color"));
    EXPECT_TRUE(s.HasUniform("albedo"));
    EXPECT_FALSE(s.HasUniform("specular"));

    EXPECT_TRUE(s.HasUniformType(Shader::Uniform::Type::Mat4));
    EXPECT_TRUE(s.HasUniformType(Shader::Uniform::Type::Vec3));
    EXPECT_TRUE(s.HasUniformType(Shader::Uniform::Type::Vec4));
    EXPECT_TRUE(s.HasUniformType(Shader::Uniform::Type::Texture2D));
    EXPECT_FALSE(s.HasUniformType(Shader::Uniform::Type::Float));
}

TEST(ShaderTest, CompileFailsWithBadGpuProgram)
{
    struct BadGpuProgram : public GpuProgram
    {
        GpuProgram& BeginCompile() override { return *this; }
        GpuProgram& AddSource(const std::string& source, Type shaderType) override { return *this; }
        void Compile() override {}
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

    struct BadRendering : public RenderingInterface
    {
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
    } bad;

    Shader s("shaders/bad", 12);
    const std::string vert = R"(
        uniform vec3 pos;
        void main(){}
    )";
    const std::string frag = "void main(){}";

    s.CompileFromSource(bad, vert, frag, "");

    // since GPU program reported handle==0, CompileFromSource should avoid extracting uniforms
    EXPECT_FALSE(s.HasUniform("pos"));
}

TEST(ShaderTest, CompileFromFile_Success)
{
    FakeRendering fake;
    Shader s("shaders/file", 20);

    const auto tmp = std::filesystem::temp_directory_path();
    const auto vpath = (tmp / "test_shader_vert.glsl").string();
    const auto fpath = (tmp / "test_shader_frag.glsl").string();

    {
        std::ofstream v(vpath);
        v << "uniform mat4 M; void main(){}";
    }
    {
        std::ofstream f(fpath);
        f << "uniform vec3 v; void main(){}";
    }

    bool ok = s.CompileFromFile(fake, vpath, fpath, "");
    EXPECT_TRUE(ok);
    EXPECT_TRUE(s.HasUniform("M"));
    EXPECT_TRUE(s.HasUniform("v"));
}

TEST(ShaderTest, CompileFromFile_FailMissingFiles)
{
    FakeRendering fake;
    Shader s("shaders/file_missing", 21);

    bool ok = s.CompileFromFile(fake, "no_such.vert", "no_such.frag", "");
    EXPECT_FALSE(ok);
}

TEST(ShaderTest, CompileWithGeometryExtractsUniforms)
{
    FakeRendering fake;
    Shader s("shaders/geom", 22);

    const std::string vert = R"(
        uniform mat4 MVP;
        void main(){}
    )";

    const std::string frag = R"(
        uniform vec4 tint;
        void main(){}
    )";

    const std::string geom = R"(
        uniform vec2 offsets;
        void main(){}
    )";

    s.CompileFromSource(fake, vert, frag, geom);

    EXPECT_TRUE(s.HasUniform("MVP"));
    EXPECT_TRUE(s.HasUniform("tint"));
    EXPECT_TRUE(s.HasUniform("offsets"));
}

TEST(ShaderTest, CompileThrowsWhenSourcesMissing)
{
    FakeRendering fake;
    Shader s("shaders/missing", 23);
    // No sources set -> Compile() should throw due to m_sources.at() usage
    EXPECT_THROW(s.Compile(fake), std::out_of_range);
}

TEST(ShaderTest, UniformParsingEdgeCases)
{
    FakeRendering fake;
    Shader s("shaders/edge", 24);

    const std::string vert = R"(
        uniform float a, b; // multiple declarators on one line - should be ignored
        uniform    mat4   Model ; // extra spaces
        uniform unknownType x; // unknown type should be ignored
        void main(){}
    )";

    s.CompileFromSource(fake, vert, "void main(){}", "");

    EXPECT_TRUE(s.HasUniform("Model"));
    EXPECT_FALSE(s.HasUniform("a"));
    EXPECT_FALSE(s.HasUniform("b"));
    EXPECT_FALSE(s.HasUniform("x"));
}
