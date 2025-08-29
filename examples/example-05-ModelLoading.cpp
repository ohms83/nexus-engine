//
// Created by nutta on 6/23/2025.
//
#include "nexus/Nexus.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

static const std::vector<std::string> modelPaths = {
    "meshes/apple/3DApple001_SQ-1K-PNG.obj",
    "meshes/armadillo/armadillo.obj",
    "meshes/bunny/stanford-bunny.obj",
    "meshes/cube/cube_textured.obj",
    "meshes/barrel/wine_barrel_01_4k.gltf",
    "meshes/mantaray/Manta_Ray.fbx",
};
static const std::vector<std::string> modelLabels = {
    "Apple",
    "Armadillo",
    "Bunny",
    "Crate",
    "Wine Barrel",
    "Manta Ray",
};
static const std::vector<glm::vec3> modelScales = {
    glm::vec3(3),
    glm::vec3(0.01),
    glm::vec3(3),
    glm::vec3(1),
    glm::vec3(1),
    glm::vec3(0.01),
};

static const char* currentLabel = modelLabels[0].c_str();
static int selectedModel = 0;
static float scale = 1.0f;

class Example_05 final : public nxs::Application
{
public:
    ~Example_05() override = default;

    void Render(nxs::RenderSystem& renderSystem) override
    {
        const auto& inputManager = nxs::InputManager::Instance();
        const glm::vec2 euler = inputManager.GetAxisValue("model_rotate") * 90.f * GetDeltaTime();
        m_euler.x += euler.x;
        m_euler.y += euler.y;

        const auto modelNode = PTR_CAST<nxs::SceneNode3D>(m_scene.GetNode("Model"));
        modelNode->Orient().value = glm::mat4_cast(glm::quat(glm::radians(m_euler)));
        modelNode->Scale().value = modelScales[selectedModel] * scale;

        m_scene.Render(renderSystem);
    }

    void DrawUI() override
    {
        ImGui::Begin("Menu");
        {
            const auto numModels = modelPaths.size();
            ImGui::SeparatorText("Model");
            if (ImGui::BeginCombo("Select Model", currentLabel) && m_numLoaded == numModels)
            {
                for (size_t n = 0; n < numModels; n++)
                {
                    const bool isSelected = (selectedModel == n);
                    if (ImGui::Selectable(modelLabels[n].c_str(), isSelected) && !isSelected)
                    {
                        currentLabel = modelLabels[n].c_str();
                        selectedModel = n;

                        auto& [model] = m_scene.GetNode("Model")->GetComponent<nxs::ModelComponent>();
                        model = m_models[selectedModel];
                    }

                    // Set the initial focus when the combo box is first opened
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SliderFloat("Scale", &scale, 1.0f, 5.0f);

            ImGui::SeparatorText("Lights");
            if (ImGui::TreeNode("Ambient"))
            {
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_ambient));
                ImGui::SliderFloat("AO Factor", &m_aoFactor, 0.0f, 1.0f);
                ImGui::TreePop();

                m_scene.Ambient() = m_ambient;
            }

            if (ImGui::TreeNode("Directional"))
            {
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_directionalLight->properties.color));
                ImGui::TreePop();
            }

            ImGui::SeparatorText("Point Lights");
            if (ImGui::TreeNode("Light 0"))
            {
                static bool enableLight = false;
                static float position[] = {0, 0, 0};
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[0]->properties.color));
                ImGui::InputFloat3("Position", position);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Light 1"))
            {
                static bool enableLight = false;
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[1]->properties.color));
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        const auto renderInterface = renderSystem.GetRenderInterface();
        renderSystem.SetClearColor(0x303030ff);

        InitScene();
        InitLights();

        const nxs::KeyInputMap modelRotationInput = {
            {
                {SDLK_LEFT, nxs::KeyInputMap::AxisPlusY},
                {SDLK_RIGHT, nxs::KeyInputMap::AxisMinusY},
                {SDLK_UP, nxs::KeyInputMap::AxisPlusX},
                {SDLK_DOWN, nxs::KeyInputMap::AxisMinusX},
            }
        };

        auto& inputManager = nxs::InputManager::Instance();
        inputManager.RegisterAxisInputMap("model_rotate", modelRotationInput);
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width = FLOAT_CAST(screenSize.x);
        m_camera.height = FLOAT_CAST(screenSize.y);

        auto camera = PTR_CAST<nxs::Camera>(m_scene.GetNode("Camera"));
        camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
    }

private:
    void InitScene()
    {
        auto camera = m_scene.CreateNode<nxs::Camera>("Camera");
        camera->Position().value = {0, 5, 5};
        camera->LookAt(glm::vec3(0), glm::vec3(0, 1, 0));

        const auto node = m_scene.CreateNode<nxs::SceneNode3D>("Model");
        node->AddComponent<nxs::ModelComponent>();

        LoadAllModels();

        m_scene.SetRenderer(std::make_unique<nxs::BasicSceneRenderer>());
    }

    void LoadAllModels()
    {
        nxs::HighResTimeSource timeSource;
        const auto now = timeSource.Now();
        const auto numModel = modelPaths.size();
        m_numLoaded = 0;

        for (int i = 0; i < numModel; i++)
        {
            m_models.emplace_back(nullptr);
        }

        auto modelManager = nxs::Engine::Instance().GetModelManager();
        for (int i = 0; i < numModel; i++)
        {
            // Preload models
            const auto assetPath = GetAssetPath(modelPaths[i]);
            modelManager->RequestResource(assetPath, [this, assetPath, &timeSource, now, i, numModel](nxs::Ref<nxs::Model> loadedModel)
            {
                NXS_ASSERT_MSG(loadedModel != nullptr, std::format("Failed to load a model file: {}", assetPath));
                std::lock_guard<std::mutex> lock(m_mutex);
                m_models[i] = loadedModel;

                if (++m_numLoaded == numModel)
                {
                    auto& [model] = m_scene.GetNode("Model")->GetComponent<nxs::ModelComponent>();
                    model = loadedModel;
                    LOG_INFO(LogTemp, std::format("Total loading time: {:.3f} seconds", timeSource.Now() - now));
                }
            });
        }
    }

    MAYBE_UNUSED nxs::Ref<nxs::Model> LoadModel(const int index)
    {
        const auto assetPath = GetAssetPath(modelPaths[index]);
        auto model = nxs::Engine::Instance().GetModelManager()->Get(assetPath);
        NXS_ASSERT_MSG(model != nullptr, std::format("Failed to load a model file: {}", assetPath));
        return model;
    }

    void InitLights()
    {
        m_scene.Ambient() = {0.5f, 0.5f, 0.5f};

        {
            auto light = m_scene.CreateNode<nxs::DirectionalLight>("Direct Light 1");
            light->SetColor({1, 1, 1});
            light->GetLightComponent().direction = {10, -10, 0};

            m_directionalLight = &light->GetLightComponent();
        }
        {
            auto light = m_scene.CreateNode<nxs::PointLight>("Point Light 1");
            light->SetColor({1, 0, 0});
            light->Position() = {5, 0, 0};

            auto& component = light->GetLightComponent();
            // component.position = {5, 0, 0};
            component.properties.cutoffRange = 100.f;
            component.constant = 0.01f;

            m_pointLights[0] = &light->GetLightComponent();
        }
        {
            auto light = m_scene.CreateNode<nxs::PointLight>("Point Light 2");
            light->SetColor({0, 0, 1});
            light->Position() = {-5, 0, 0};

            auto& component = light->GetLightComponent();
            // component.position = {-5, 0, 0};
            component.properties.cutoffRange = 100.f;
            component.constant = 0.01f;

            m_pointLights[1] = &light->GetLightComponent();
        }
    }

protected:
    nxs::Scene m_scene;
    nxs::Ref<nxs::GpuProgram> m_gpuProgram;
    //! Currently showing model
    nxs::Ref<nxs::Model> m_model;
    nxs::Transform m_cubeTransform;
    nxs::CameraComponent m_camera;
    nxs::DirectLightComponent* m_directionalLight = nullptr;
    nxs::PointLightComponent* m_pointLights[2] {};
    nxs::Color3F m_ambient {0.5, 0.5, 0.5};
    glm::vec3 m_euler {};
    glm::vec3 m_cameraPos {0, 5, 5};
    float m_aoFactor = 1;
    std::vector<nxs::Ref<nxs::Model>> m_models;
    size_t m_numLoaded = 0;
    //! Mutex for thread-safe access to m_models
    mutable std::mutex m_mutex;
};


int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nxs::RunApplication<Example_05>({
        "Example 05 - Model Loading",
        graphicsConfig,
        fullscreen,
        true
    });
}
