#include "nexus/Nexus.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/TextureBuilder.h"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace
{
    constexpr int kTextureSize = 64;
    constexpr int kMipCount = 7;

    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 6) in vec2 aTexCoord0;

out vec2 texCoord0;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord0 = aTexCoord0;
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 texCoord0;
uniform sampler2D mipTexture;

void main()
{
    FragColor = texture(mipTexture, texCoord0);
}
)";

    std::vector<uint8_t> CreateBaseTextureData(const int size)
    {
        std::vector<uint8_t> data(static_cast<size_t>(size) * size * 4);
        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                const size_t offset = (static_cast<size_t>(y) * size + x) * 4;
                const bool checker = ((x / 16) + (y / 16)) & 1;
                data[offset] = checker ? 245 : 40;
                data[offset + 1] = checker ? 180 : 70;
                data[offset + 2] = checker ? 30 : 220;
                data[offset + 3] = 255;
            }
        }
        return data;
    }

    std::vector<uint8_t> DownsampleMip(const std::vector<uint8_t>& source, const int sourceSize)
    {
        const int mipSize = std::max(1, sourceSize / 2);
        std::vector<uint8_t> data(static_cast<size_t>(mipSize) * mipSize * 4);

        for (int y = 0; y < mipSize; ++y)
        {
            for (int x = 0; x < mipSize; ++x)
            {
                uint32_t red = 0;
                uint32_t green = 0;
                uint32_t blue = 0;
                uint32_t alpha = 0;
                uint32_t sampleCount = 0;

                for (int sampleY = y * 2; sampleY < std::min(sourceSize, y * 2 + 2); ++sampleY)
                {
                    for (int sampleX = x * 2; sampleX < std::min(sourceSize, x * 2 + 2); ++sampleX)
                    {
                        const size_t sourceOffset =
                            (static_cast<size_t>(sampleY) * sourceSize + sampleX) * 4;
                        red += source[sourceOffset];
                        green += source[sourceOffset + 1];
                        blue += source[sourceOffset + 2];
                        alpha += source[sourceOffset + 3];
                        ++sampleCount;
                    }
                }

                const size_t offset = (static_cast<size_t>(y) * mipSize + x) * 4;
                data[offset] = CAST<uint8_t>(red / sampleCount);
                data[offset + 1] = CAST<uint8_t>(green / sampleCount);
                data[offset + 2] = CAST<uint8_t>(blue / sampleCount);
                data[offset + 3] = CAST<uint8_t>(alpha / sampleCount);
            }
        }
        return data;
    }
}

class Example_11 final : public nxs::Application
{
public:
    ~Example_11() override = default;

    void DrawUI() override
    {
        ImGui::Begin("Mipmap Comparison");
        {
            ImGui::Checkbox("Use mipmapped texture", &m_useMipmaps);
            ImGui::Separator();
            ImGui::TextUnformatted(m_useMipmaps
                ? "Mipmapped: 7 manually uploaded mip levels"
                : "No mipmaps: mip level 0 only");
        }
        ImGui::End();
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        const glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 5.0f, 20.0f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(
            glm::radians(m_camera.fov),
            m_camera.width / m_camera.height,
            m_camera.nearZ,
            m_camera.farZ);

        m_gpuProgram->Bind();
        m_gpuProgram->SetUniformMatrix(
            "model",
            nxs::Matrix::CreateModelMatrix(glm::vec3(0.0f, -1.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f)),
            false);
        m_gpuProgram->SetUniformMatrix("view", view, false);
        m_gpuProgram->SetUniformMatrix("projection", projection, false);
        m_gpuProgram->SetUniformTexture2D(
            "mipTexture",
            m_useMipmaps ? m_mipmappedTexture : m_nonMipmapTexture,
            0);

        m_mesh->GetVertexBuffer()->Bind();
        m_mesh->GetIndexBuffer()->Bind();
        renderSystem.DrawIndexed(m_mesh->GetIndexBuffer());
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        renderSystem.GetRenderInterface()->SetClearColor(0x303030ff);

        m_gpuProgram = nxs::GetGpuResourceFactory()->CreateGpuProgram();
        m_gpuProgram->BeginCompile()
            .AddSource(vertexShaderSource, nxs::GpuProgram::Type::Vertex)
            .AddSource(fragmentShaderSource, nxs::GpuProgram::Type::Fragment)
            .Compile();

        nxs::TextureDescription mipmappedDescription {
            .width = kTextureSize,
            .height = kTextureSize,
            .kind = nxs::TextureKind::Texture2D,
            .channels = 4,
            .format = nxs::PixelFormat::RGBA,
            .componentType = nxs::DataType::UByte,
            .wrapModeS = nxs::TextureWrapMode::Repeat,
            .wrapModeT = nxs::TextureWrapMode::Repeat,
            .filterMin = nxs::TextureFilterMode::LinearMipmapLinear,
            .filterMag = nxs::TextureFilterMode::Linear,
            .numMips = kMipCount,
        };

        nxs::TextureBuilder builder;
        builder.Begin(nxs::GetGpuResourceFactory(), mipmappedDescription);
        auto currentMipData = CreateBaseTextureData(kTextureSize);
        for (int mip = 0; mip < kMipCount; ++mip)
        {
            builder.LoadMipData(
                currentMipData.data(),
                static_cast<uint32_t>(currentMipData.size()),
                static_cast<uint32_t>(mip));
            if (mip + 1 < kMipCount)
            {
                const int currentSize = std::max(1, kTextureSize >> mip);
                currentMipData = DownsampleMip(currentMipData, currentSize);
            }
        }
        m_mipmappedTexture = builder.End();

        nxs::TextureDescription nonMipmapDescription = mipmappedDescription;
        nonMipmapDescription.numMips = 1;
        nonMipmapDescription.filterMin = nxs::TextureFilterMode::Linear;

        auto mip0Data = CreateBaseTextureData(kTextureSize);
        nxs::TextureBuilder nonMipmapBuilder;
        m_nonMipmapTexture = nonMipmapBuilder.Begin(nxs::GetGpuResourceFactory(), nonMipmapDescription)
            .LoadMipData(mip0Data.data(), static_cast<uint32_t>(mip0Data.size()), 0)
            .End();

        m_mesh = nxs::PrimitiveMesh::CreatePlane(
            "Mipmap Comparison Plane",
            100.0f,
            100.0f,
            *nxs::GetGpuResourceFactory(),
            50
        );
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width = FLOAT_CAST(screenSize.x);
        m_camera.height = FLOAT_CAST(screenSize.y);
    }

    nxs::Ref<nxs::GpuProgram> m_gpuProgram;
    nxs::Ref<nxs::TextureProxy> m_mipmappedTexture;
    nxs::Ref<nxs::TextureProxy> m_nonMipmapTexture;
    nxs::Ref<nxs::Mesh> m_mesh;
    nxs::CameraComponent m_camera;
    bool m_useMipmaps = true;
};

int main()
{
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280,
        960,
        false,
    };
    return nxs::RunApplication<Example_11>({
        "Example 11 - Mipmaps",
        graphicsConfig,
        true,
    });
}
