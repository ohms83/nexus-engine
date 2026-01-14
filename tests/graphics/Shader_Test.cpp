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
    BadRendering bad;

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
        uniform float a, b, c; // multiple declarators on one line - this should be handled
        uniform vec3 d,e , f; // various spaces
        uniform vec4 g,h;uniform vec2 i; // no line breaks
        uniform vec3 j /* inline block comment */, k; // the comment should be ignored
        /** 
         * Multi lines block comment.
         * uniform float l; must be ignored.
         */
        // uniform float m; inline comment should be ignored
        uniform    mat4   _Model ; // extra spaces
        uniform unknownType x; // unknown type should be ignored
        void main(){}
    )";

    s.CompileFromSource(fake, vert, "void main(){}", "");

    EXPECT_TRUE(s.HasUniformType(Shader::Uniform::Type::Mat4));
    EXPECT_TRUE(s.HasUniform("_Model"));

    EXPECT_TRUE(s.HasUniform("a"));
    EXPECT_TRUE(s.HasUniform("b"));
    EXPECT_TRUE(s.HasUniform("c"));
    EXPECT_TRUE(s.HasUniform("d"));
    EXPECT_TRUE(s.HasUniform("e"));
    EXPECT_TRUE(s.HasUniform("f"));
    EXPECT_TRUE(s.HasUniform("g"));
    EXPECT_TRUE(s.HasUniform("h"));
    EXPECT_TRUE(s.HasUniform("i"));
    EXPECT_TRUE(s.HasUniform("j"));
    EXPECT_TRUE(s.HasUniform("k"));

    EXPECT_FALSE(s.HasUniform("l"));
    EXPECT_FALSE(s.HasUniform("m"));
    EXPECT_FALSE(s.HasUniform("x"));
    // Check whether the parser didn't mistakenly include parts of initializers/comments
    EXPECT_FALSE(s.HasUniform("vec3(0.0)"));
    EXPECT_FALSE(s.HasUniform("1.0"));
    EXPECT_FALSE(s.HasUniform("/* inline comment */"));
}

class ShaderGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempDir = std::filesystem::temp_directory_path() / "test_shaders";
        genDir = tempDir / "gen";

        try {
            if (std::filesystem::exists(tempDir)) {
                std::filesystem::remove_all(tempDir);
            }
            std::filesystem::create_directory(tempDir);
            // genDir is created by ShaderGenerator when writing files.
        }
        catch (const std::filesystem::filesystem_error& ex) {
            std::cerr << "Filesystem error: " << ex.what() << std::endl;
            return;
        }
        std::filesystem::current_path(tempDir);

        shaderGen.SetOutputDirectory(genDir.string());
    }

    void TearDown() override {
        try {
            if (std::filesystem::exists(tempDir)) {
                std::filesystem::remove_all(tempDir);
            }
        }
        catch (const std::filesystem::filesystem_error& ex) {
            std::cerr << "Filesystem error during teardown: " << ex.what() << std::endl;
        }
    }

    void CreateFile(const std::string& path, const std::string& content) {
        std::ofstream ofs((tempDir / path).string());
        ofs << content;
        ofs.close();
    }

    std::string GetFilePath(const std::string& filename) {
        return (tempDir / filename).string();
    }

    ShaderGenerator shaderGen;
    std::filesystem::path tempDir;
    std::filesystem::path genDir;
};

TEST_F(ShaderGeneratorTest, BasicVertexFragmentGeneration) {
    CreateFile("basic.shader", 
        "@glsl_version 450 core\n"
        "@program vertex\n"
        "layout(location = 0) in vec3 aPos;\n"
        "@endprogram\n"
        "@program fragment\n"
        "out vec4 FragColor;\n"
        "@endprogram");

    std::string vs, fs, gs;
    const auto path = GetFilePath("basic.shader");
    bool success = shaderGen.GenerateShaderSource(path, vs, fs, gs);

    EXPECT_TRUE(success);
    EXPECT_TRUE(vs.find("#version 450 core") != std::string::npos);
    EXPECT_TRUE(vs.find("aPos") != std::string::npos);
    EXPECT_TRUE(fs.find("FragColor") != std::string::npos);
    EXPECT_EQ(gs, "");
}

TEST_F(ShaderGeneratorTest, HandlesInvalidIncludePath) {
    CreateFile("main.shader", 
        "@glsl_version 330\n"
        "@program vertex\n"
        "@include \"math.glsl\"\n"
        "@endprogram");

    std::string vs, fs, gs;
    const auto path = GetFilePath("main.shader");
    bool success = shaderGen.GenerateShaderSource(path, vs, fs, gs);

    EXPECT_FALSE(success);
}

TEST_F(ShaderGeneratorTest, HandlesNestedIncludes) {
    CreateFile("consts.glsl", "#define PI 3.14");
    CreateFile("math.glsl", "@include \"consts.glsl\"\nfloat getPi() { return PI; }");
    CreateFile("main.shader", 
        "@glsl_version 330\n"
        "@program vertex\n"
        "@include \"math.glsl\"\n"
        "@endprogram");

    std::string vs, fs, gs;
    const auto path = GetFilePath("main.shader");
    bool success = shaderGen.GenerateShaderSource(path, vs, fs, gs);

    EXPECT_TRUE(success);
    EXPECT_TRUE(vs.find("#define PI 3.14") != std::string::npos);
    EXPECT_TRUE(vs.find("float getPi()") != std::string::npos);
}

TEST_F(ShaderGeneratorTest, FailsIfNoVersionSpecified) {
    CreateFile("invalid.shader", 
        "@program vertex\n"
        "void main() {}\n"
        "@endprogram");

    std::string vs, fs, gs;
    const auto path = GetFilePath("invalid.shader");
    bool success = shaderGen.GenerateShaderSource(path, vs, fs, gs);

    EXPECT_FALSE(success);
}

TEST_F(ShaderGeneratorTest, FailsUnknownProgramType) {
    CreateFile("invalid.shader", 
        "@glsl_version 330\n"
        "@program vert\n"
        "void main() {}\n"
        "@endprogram");

    std::string vs, fs, gs;
    const auto path = GetFilePath("invalid.shader");
    bool success = shaderGen.GenerateShaderSource(path, vs, fs, gs);

    EXPECT_FALSE(success);
}

TEST_F(ShaderGeneratorTest, WritesGeneratedFilesToDisk) {
    // 1. Setup paths
    genDir = (tempDir / "gen").string();
    shaderGen.SetOutputDirectory(genDir.string());

    // 2. Create mock source with include
    CreateFile("math.glsl", "#define PI 3.14");
    CreateFile("test.shader", 
        "@glsl_version 330\n"
        "@program vertex\n"
        "@include \"math.glsl\"\n"
        "void main() {}\n"
        "@endprogram");

    // 3. Execute
    std::string vs, fs, gs;
    bool success = shaderGen.GenerateShaderSource(GetFilePath("test.shader"), vs, fs, gs);

    // 4. Assertions
    EXPECT_TRUE(success);
    
    // Check if file was actually written to the 'gen' directory
    std::filesystem::path expectedPath = genDir / "test.vert";
    EXPECT_TRUE(std::filesystem::exists(expectedPath));

    // Verify content of the written file
    std::ifstream ifs(expectedPath);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("#version 330") != std::string::npos);
    EXPECT_TRUE(content.find("#define PI 3.14") != std::string::npos);
}

TEST_F(ShaderGeneratorTest, HandlesInvalidOutputDirectory) {
    // Create a file where a directory should be to block directory creation
    CreateFile("blocked_path", "I am a file, not a folder");
    CreateFile("test.shader", 
        "@glsl_version 330\n"
        "@program vertex\n"
        "void main() {}\n"
        "@endprogram");

    shaderGen.SetOutputDirectory(GetFilePath("blocked_path"));

    // This should not crash, but should log an error internally
    std::string vs, fs, gs;
    bool success = shaderGen.GenerateShaderSource(GetFilePath("test.shader"), vs, fs, gs);
    
    // The source generation should still succeed even if the disk-write fails
    EXPECT_TRUE(success); 
}