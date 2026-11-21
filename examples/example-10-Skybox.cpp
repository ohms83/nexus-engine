#include "nexus/Nexus.h"
#include "nexus/graphics/CubeTexture.h"
#include "nexus/memory/OwningBuffer.h"

#include <stb_image.h>

class Example_10 final : public nxs::Application
{
public:
    ~Example_10() override = default;

    void Render(nxs::RenderSystem& renderSystem) override
    {
        if (m_scene) m_scene->Render(renderSystem);
    }

protected:
    bool Init_Internal() override
    {
        auto sceneManager = nxs::Engine::Instance().GetSceneManager();
        m_scene = sceneManager->EmplaceAndChange<nxs::Scene>("Skybox Scene");

        auto camera = m_scene->EmplaceChild<nxs::Camera>("Camera");
        camera->SetPosition(glm::vec3(0.0f));
        camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        m_scene->SetSkybox(LoadSkybox());
        m_scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem()));
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width = FLOAT_CAST(screenSize.x);
        m_camera.height = FLOAT_CAST(screenSize.y);

        auto camera = PTR_CAST<nxs::Camera>(m_scene->FindNodeWithName("Camera"));
        if (camera)
        {
            camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
        }
    }

private:
    nxs::Ref<nxs::CubeTexture> LoadSkybox()
    {
        static const nxs::Array<std::string> faceNames = {
            "px.png",
            "nx.png",
            "py.png",
            "ny.png",
            "pz.png",
            "nz.png",
        };
        static const std::string basePath = "textures/skybox/sky_93_2k/sky_93_cubemap_2k/";

        int width = 0;
        int height = 0;
        stbi_set_flip_vertically_on_load(0);

        const std::string firstPath = GetAssetPath(basePath + faceNames.front());
        int channels = 0;
        uint8_t* firstPixels = stbi_load(firstPath.c_str(), &width, &height, &channels, 3);
        if (!firstPixels)
        {
            LOG_ERROR(LogTemp, std::format("Failed to load skybox face: {}", firstPath));
            return nullptr;
        }
        stbi_image_free(firstPixels);

        auto cubemap = nxs::GetGpuResourceFactory()->CreateCubeTexture(width, height);
        if (!cubemap)
        {
            LOG_ERROR(LogTemp, "Failed to create skybox cubemap.");
            return nullptr;
        }

        cubemap->Bind();
        for (uint32_t i = 0; i < faceNames.size(); ++i)
        {
            const std::string path = GetAssetPath(basePath + faceNames[i]);
            uint8_t* pixels = stbi_load(path.c_str(), &width, &height, &channels, 3);
            if (!pixels)
            {
                LOG_ERROR(LogTemp, std::format("Failed to load skybox face: {}", path));
                cubemap->Unbind();
                return nullptr;
            }

            const uint32_t size = UINT_CAST(width) * UINT_CAST(height) * 3;
            cubemap->LoadData(CAST<nxs::CubeTexture::Face>(i), pixels, size);
            stbi_image_free(pixels);
        }

        cubemap->Unbind();
        return cubemap;
    }

    nxs::Ref<nxs::Scene> m_scene;
    nxs::CameraComponent m_camera;
};

int main()
{
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1960,
        1080,
        true,
    };
    return nxs::RunApplication<Example_10>({
        "Example 10 - Skybox",
        graphicsConfig,
        false,
    });
}
