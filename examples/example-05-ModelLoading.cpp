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
};
static const std::vector<std::string> modelLabels = {
    "Apple",
    "Armadillo",
    "Bunny",
    "Crate",
    "Wine Barrel",
};
static const std::vector<glm::vec3> modelScales = {
    glm::vec3(3),
    glm::vec3(0.01),
    glm::vec3(3),
    glm::vec3(1),
    glm::vec3(1),
};

static const char* currentLabel = modelLabels[0].c_str();
static int selectedModel = 0;
static float scale = 1.0f;
static bool drawBox = false;
static bool drawSphere = false;

class Example_05 final : public nxs::Application
{
public:
    ~Example_05() override = default;

    void Update() override
    {
        if (m_finishLoading) return;

        for (const auto & m_loadedModel : m_loadedModels)
        {
            if (m_loadedModel->status != nxs::IResourceLoader::LoadResult::Status::Ready) return;
        }

        auto& modelComp = m_scene.FindNode("Model")->GetComponent<nxs::ModelComponent>();
        modelComp.model = PTR_CAST<nxs::Model>(m_loadedModels[selectedModel]->resource);
        m_finishLoading = true;
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        const auto& inputManager = nxs::InputManager::Instance();
        const glm::vec2 euler = inputManager.GetAxisValue("model_rotate") * 90.f * GetDeltaTime();
        m_euler.x += euler.x;
        m_euler.y += euler.y;

        auto modelNode = PTR_CAST<nxs::SceneNode3D>(m_scene.FindNode("Model"));
        modelNode->Orient().value = glm::mat4_cast(glm::quat(glm::radians(m_euler)));
        modelNode->Scale().value = modelScales[selectedModel] * scale;

        auto modelComp = modelNode->TryGetComponent<nxs::ModelComponent>();
        if (modelComp)
        {
            const auto model = modelComp->model;
            const auto position = modelNode->Position().value;
            const auto orient = modelNode->Orient().value;
            const float scale = modelNode->Scale().value.x;
            if (drawSphere)
            {
                auto sphere = model->GetBoundingSphere();
                nxs::Gizmos::DrawOutlineSphere(renderSystem, (position + sphere.center) * scale, sphere.radius * scale);
            }
            if (drawBox)
            {
                auto box = model->GetBoundingBox();
                nxs::Gizmos::DrawOutlineBox(renderSystem, (position + box.position) * scale, box.extent * scale);
            }
        }

        m_scene.Render(renderSystem);
    }

    void DrawUI() override
    {
        ImGui::Begin("Menu");
        {
            ImGui::SeparatorText("Model");
            if (ImGui::BeginCombo("Select Model", currentLabel))
            {
                for (size_t n = 0; n < modelLabels.size(); n++)
                {
                    const bool isSelected = (selectedModel == n);
                    if (ImGui::Selectable(modelLabels[n].c_str(), isSelected) && !isSelected)
                    {
                        currentLabel = modelLabels[n].c_str();
                        selectedModel = n;

                        if (m_loadedModels[n]->status == nxs::IResourceLoader::LoadResult::Status::Ready)
                        {
                            auto& modelComp = m_scene.FindNode("Model")->GetComponent<nxs::ModelComponent>();
                            modelComp.model = PTR_CAST<nxs::Model>(m_loadedModels[n]->resource);
                        }
                    }

                    // Set the initial focus when the combo box is first opened
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SliderFloat("Scale", &scale, 1.0f, 5.0f);
            ImGui::Checkbox("Draw Sphere", &drawSphere);
            ImGui::Checkbox("Draw Box", &drawBox);

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
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_directionalLight->Properties().color));
                ImGui::TreePop();
            }

            ImGui::SeparatorText("Point Lights");
            if (ImGui::TreeNode("Light 0"))
            {
                static bool enableLight = false;
                static float position[] = {0, 0, 0};
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[0]->Properties().color));
                ImGui::InputFloat3("Position", position);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Light 1"))
            {
                static bool enableLight = false;
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[1]->Properties().color));
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

        const auto engine = nxs::Engine::Instance();
        m_modelLoader = std::make_unique<nxs::ModelLoader>(
            renderInterface,
            engine.GetTextureManager(),
            engine.GetMaterialManager()
        );

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

        auto camera = PTR_CAST<nxs::Camera>(m_scene.FindNode("Camera"));
        camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
    }

private:
    void InitScene()
    {
        auto camera = m_scene.CreateNode<nxs::Camera>("Camera");
        camera->Position().value = {0, 0, 5};
        camera->LookAt(glm::vec3(0), glm::vec3(0, 0, 0));

        for (int i = 0; i < modelPaths.size(); i++)
        {
            // Preload models
            m_loadedModels.emplace_back(LoadModel(i));
        }

        auto node = m_scene.CreateNode<nxs::SceneNode3D>("Model");
        node->AddComponent<nxs::ModelComponent>();
        node->Scale().value = modelScales[0];

        m_scene.SetRenderer(std::make_unique<nxs::BasicSceneRenderer>());
    }

    MAYBE_UNUSED nxs::Ref<nxs::IResourceLoader::LoadResult> LoadModel(const int index)
    {
        const auto taskScheduler = nxs::Engine::Instance().GetTaskScheduler();
        const auto assetPath = GetAssetPath(modelPaths[index]);
        auto result = m_modelLoader->LoadAsync(assetPath, index, *taskScheduler, [this](nxs::Ref<nxs::Resource> model) {});

        taskScheduler->ScheduleTask(std::make_shared<nxs::RepeatTask>(-1, [result]() -> bool
        {
            if (result->status == nxs::IResourceLoader::LoadResult::Status::Ready)
            {
                LOG_INFO(LogTemp, std::format("Finished loading {}", result->path));
                return false;
            }
            return true;
        }));
        return result;
    }

    void InitLights()
    {
        m_scene.Ambient() = {0.5f, 0.5f, 0.5f};

        {
            auto light = m_scene.CreateNode<nxs::DirectionalLight>("Direct Light 1");
            light->SetColor({1, 1, 1});
            light->Direction() = {10, -10, 0};
            m_directionalLight = light;
        }
        {
            auto light = m_scene.CreateNode<nxs::PointLight>("Point Light 1");
            light->SetColor({1, 0, 0});
            light->Position() = {5, 0, 0};
            light->Properties().cutoffRange = 100.f;
            light->SetConstantAttenuation(0.01f);

            m_pointLights[0] = light;
        }
        {
            auto light = m_scene.CreateNode<nxs::PointLight>("Point Light 2");
            light->SetColor({0, 0, 1});
            light->Position() = {-5, 0, 0};
            light->Properties().cutoffRange = 100.f;
            light->SetConstantAttenuation(0.01f);

            m_pointLights[1] = light;
        }
    }

protected:
    nxs::Scene m_scene;
    nxs::Transform m_cubeTransform;
    nxs::CameraComponent m_camera;
    nxs::Ref<nxs::PointLight> m_pointLights[2] {};
    nxs::Ref<nxs::DirectionalLight> m_directionalLight;
    nxs::Color3F m_ambient {0.5, 0.5, 0.5};
    glm::vec3 m_euler {};
    glm::vec3 m_cameraPos {0, 5, 5};
    nxs::Ptr<nxs::ModelLoader> m_modelLoader;
    std::vector<nxs::Ref<nxs::IResourceLoader::LoadResult>> m_loadedModels;
    float m_aoFactor = 1;
    bool m_finishLoading = false;
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
