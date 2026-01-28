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

class Example_06 final : public nxs::Application
{
public:
    ~Example_06() override = default;

    void Update() override
    {
        const auto selectedNode = m_sceneGraphWidget->GetSelectedNode();
        m_propertyWindow->SetSceneNode(selectedNode);

        if (m_finishLoading)
        {
            const auto& inputManager = nxs::InputManager::Instance();
            const glm::vec2 euler = inputManager.GetAxisValue("model_rotate") * 90.f * GetDeltaTime();
            m_euler.x += euler.x;
            m_euler.y += euler.y;

            auto modelNode = PTR_CAST<nxs::SceneNode3D>(m_scene->FindNodeWithName("Model"));
            if (!modelNode) return;

            modelNode->Orient().quat = glm::mat4_cast(glm::quat(glm::radians(m_euler)));
            modelNode->Scale().value = modelScales[selectedModel] * scale;
            return;
        }

        for (const auto & m_loadedModel : m_loadedModels)
        {
            if (m_loadedModel->status != nxs::IResourceLoader::LoadResult::Status::Ready) return;
        }

        auto modelComp = m_scene->FindNodeWithName("Model")->GetComponent<nxs::ModelComponent>();
        if (!modelComp) return;

        modelComp->SetModel(PTR_CAST<nxs::Model>(m_loadedModels[selectedModel]->resource));
        m_finishLoading = true;
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        if (!m_scene) return;
        m_scene->Render(renderSystem);
    }

    void DrawUI() override
    {
        ImGui::Begin("Model Settings");
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
                            auto modelComp = m_scene->FindNodeWithName("Model")->GetComponent<nxs::ModelComponent>();
                            modelComp->SetModel(PTR_CAST<nxs::Model>(m_loadedModels[n]->resource));
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
        }
        ImGui::End();

        auto renderSystem = nxs::Engine::Instance().GetRenderSystem();
        m_propertyWindow->Draw(*renderSystem);
        m_sceneGraphWidget->Draw(*renderSystem);
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        const auto renderInterface = renderSystem.GetRenderInterface();

        const auto engine = nxs::Engine::Instance();
        m_modelLoader = std::make_unique<nxs::ModelLoader>(
            renderInterface,
            engine.GetResourceManager()
        );

        auto sceneManager = engine.GetSceneManager();
        m_scene = sceneManager->EmplaceAndChange<nxs::Scene>("Main Scene");
        sceneManager->sceneChangedCallback.connect([this](nxs::Ref<nxs::Scene> prev, nxs::Ref<nxs::Scene> next) {
            m_scene = next;
        });

        InitScene();
        InitLights();

        m_sceneGraphWidget = std::make_shared<nxs::SceneGraphWidget>(*sceneManager);
        m_propertyWindow = std::make_shared<nxs::PropertyWindow>(*sceneManager);

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

        auto camera = PTR_CAST<nxs::Camera>(m_scene->FindNodeWithName("Camera"));
        camera->SetProjection(m_camera.fov, m_camera.width, m_camera.height, m_camera.nearZ, m_camera.farZ);
    }

private:
    void InitScene()
    {
        auto camera = m_scene->EmplaceChild<nxs::Camera>("Camera");
        camera->Position().value = {5, 5, 5};
        camera->LookAt(glm::vec3(0), glm::vec3(0, 1, 0));

        for (int i = 0; i < modelPaths.size(); i++)
        {
            // Preload models
            m_loadedModels.emplace_back(LoadModel(i));
        }

        auto node = m_scene->EmplaceChild<nxs::SceneNode3D>("Model");
        node->AddComponent<nxs::ModelComponent>();
        node->Scale().value = modelScales[0];

        auto material = std::make_shared<nxs::Material>("Default Material", 0);
        material->CreateDefaultShader(*nxs::Engine::Instance().GetResourceManager());

        auto groundModel = nxs::Model::CreateFromMesh(
            "Ground Plane",
            nxs::PrimitiveMesh::CreatePlane(
                "Ground Plane",
                10, 10,
                *GetRenderSystem().GetRenderInterface(),
                material
            )
        );
        auto modelComp = m_scene->EmplaceChild<nxs::SceneNode3D>("Ground")->AddComponent<nxs::ModelComponent>();
        modelComp->SetModel(groundModel);

        m_scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem()));
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
        m_scene->Ambient() = {0.5f, 0.5f, 0.5f};

        {
            auto light = m_scene->EmplaceChild<nxs::DirectionalLight>("Direct Light");
            light->Properties().color =  {1, 1, 1};
            light->Direction() = {10, -10, 0};
        }
    }

protected:
    nxs::Ref<nxs::Scene> m_scene;
    nxs::CameraComponent m_camera;
    nxs::Color3F m_ambient {0.5, 0.5, 0.5};
    glm::vec3 m_euler {};
    glm::vec3 m_cameraPos {0, 5, 5};
    nxs::Ptr<nxs::ModelLoader> m_modelLoader;
    std::vector<nxs::Ref<nxs::IResourceLoader::LoadResult>> m_loadedModels;
    nxs::Ref<nxs::SceneGraphWidget> m_sceneGraphWidget;
    nxs::Ref<nxs::PropertyWindow> m_propertyWindow;
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
    return nxs::RunApplication<Example_06>({
        "Example 06 - Shadow Mapping",
        graphicsConfig,
        fullscreen,
        true,
        // editMode
        true,
        // maximize
        true,
        // allowProfile
        false,
    });
}
