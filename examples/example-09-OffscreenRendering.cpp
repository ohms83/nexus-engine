#include "nexus/Nexus.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/TextureProxy.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

class Example_09 final : public nxs::Application
{
public:
    ~Example_09() override = default;

    void Render(nxs::RenderSystem& renderSystem) override
    {
        m_cubeOrientation.Rotate(
            45.f * GetDeltaTime(),
            glm::normalize(glm::vec3(0.5f, 1.0f, 0.25f)));

        if (m_cube)
        {
            m_cube->SetOrientation(m_cubeOrientation.GetQuaternion());
        }

        if (m_scene)
        {
            m_scene->Render(renderSystem);
        }
    }

    void DrawUI() override
    {
        ImGui::Begin("Offscreen Render");
        if (m_offscreenTarget)
        {
            const auto colorTexture = m_offscreenTarget->GetColorTextureProxy();
            if (colorTexture)
            {
                const auto textureId = static_cast<ImTextureID>(
                    static_cast<uintptr_t>(colorTexture->GetHandle()));
                ImGui::Text("512 x 512 color render target");
                ImGui::Image(textureId, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        renderSystem.GetRenderInterface()->SetClearColor(0x202020ff);

        m_scene = std::make_shared<nxs::Scene>("Offscreen Scene");
        m_scene->Ambient() = {0.25f, 0.25f, 0.25f};

        auto camera = m_scene->EmplaceChild<nxs::Camera>("Offscreen Camera");
        camera->SetPosition({3.5f, 3.0f, 5.0f});
        camera->LookAt(glm::vec3(0), glm::vec3(0, 1, 0));
        camera->SetProjection(45.f, 512.f, 512.f, 0.1f, 100.f);

        m_offscreenTarget = nxs::GetGpuResourceFactory()->CreateRenderTarget(512, 512, false);
        camera->SetRenderTarget(m_offscreenTarget);
        camera->Properties().profile.clearColor = nxs::Color3F::Black;

        auto material = std::make_shared<nxs::Material>("Offscreen Cube", 0);
        material->diffuse = {0.25f, 0.65f, 1.0f};
        material->depthTest = true;
        material->depthWrite = true;
        material->cull = true;
        material->CreateDefaultShader(*nxs::Engine::Instance().GetResourceManager());

        m_cube = m_scene->EmplaceChild<nxs::SceneNode>("Cube");
        m_cube->AddComponent<nxs::MeshComponent>()->SetMesh(
            nxs::PrimitiveMesh::CreateBox(
                "Offscreen Cube",
                glm::vec3(1.5f),
                *nxs::GetGpuResourceFactory(),
                1.0f,
                material));

        auto light = m_scene->EmplaceChild<nxs::DirectionalLight>("Sun");
        light->Properties().color = {1.f, 1.f, 1.f};
        light->Direction() = {0.5f, -1.f, 0.25f};

        m_scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(renderSystem));
        return true;
    }

private:
    nxs::Ref<nxs::Scene> m_scene;
    nxs::Ref<nxs::SceneNode> m_cube;
    nxs::Ref<nxs::RenderTarget> m_offscreenTarget;
    nxs::TransformComponent m_cubeOrientation;
};

int main()
{
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1960,
        1024,
        true,
    };
    return nxs::RunApplication<Example_09>({
        "Example 09 - Offscreen Rendering",
        graphicsConfig,
        false,
    });
}
