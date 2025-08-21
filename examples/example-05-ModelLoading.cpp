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
        auto model = glm::mat4(1.0f);

        const auto& inputManager = nxs::InputManager::Instance();
        const glm::vec2 euler = inputManager.GetAxisValue("model_rotate") * 90.f * GetDeltaTime();
        m_euler.x += euler.x;
        m_euler.y += euler.y;
        model *= glm::scale(model, modelScales[selectedModel] * scale);
        model *= glm::mat4_cast(glm::quat(glm::radians(m_euler)));

        glm::mat4 view = glm::lookAt(m_cameraPos, // Camera position
                                     glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                                     glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
                                    );

        glm::mat4 projection = glm::perspective(glm::radians(m_camera.fov), m_camera.width / m_camera.height, m_camera.nearZ, m_camera.farZ);

        auto renderCommands = m_model->CreateDrawCommand();
        for (auto & renderCommand : renderCommands)
        {
            renderCommand.uniformVec3.emplace_back("_CameraPos", m_cameraPos);

            renderCommand.uniformMatrices.emplace("_Model", model);
            renderCommand.uniformMatrices.emplace("_View", view);
            renderCommand.uniformMatrices.emplace("_Projection", projection);

            renderCommand.uniformVec3.emplace_back("_AmbientLight", m_ambient);

            renderCommand.uniformVec3.emplace_back("_DirectLight.direction", m_directionalLight.direction);
            renderCommand.uniformVec3.emplace_back("_DirectLight.properties.color", m_directionalLight.properties.color);
            renderCommand.uniformFloats.emplace_back("_DirectLight.properties.diffuseIntensity", 1);
            renderCommand.uniformFloats.emplace_back("_DirectLight.properties.specularIntensity", 1);

            renderCommand.uniformVec3.emplace_back("_PointLights[0].position", m_pointLights[0].position);
            renderCommand.uniformVec3.emplace_back("_PointLights[0].properties.color", m_pointLights[0].properties.color);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].properties.diffuseIntensity", 1);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].properties.specularIntensity", 1);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].cutoff", m_pointLights[0].properties.cutoffRange);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].constant", m_pointLights[0].constant);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].linear", m_pointLights[0].linear);
            renderCommand.uniformFloats.emplace_back("_PointLights[0].quadratic", m_pointLights[0].quadratic);

            renderCommand.uniformVec3.emplace_back("_PointLights[1].position", m_pointLights[1].position);
            renderCommand.uniformVec3.emplace_back("_PointLights[1].properties.color", m_pointLights[1].properties.color);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].properties.diffuseIntensity", 1);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].properties.specularIntensity", 1);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].cutoff", m_pointLights[1].properties.cutoffRange);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].constant", m_pointLights[1].constant);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].linear", m_pointLights[1].linear);
            renderCommand.uniformFloats.emplace_back("_PointLights[1].quadratic", m_pointLights[1].quadratic);

            renderCommand.uniformFloats.emplace_back("_AOFactor", m_aoFactor);
            renderCommand.uniformInts.emplace_back("_NumPointLight", 2);

            renderSystem.RegisterDrawCommand(renderCommand);
        }
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
                        LoadModel(selectedModel);
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
            }

            if (ImGui::TreeNode("Directional"))
            {
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_directionalLight.properties.color));
                ImGui::TreePop();
            }

            ImGui::SeparatorText("Point Lights");
            if (ImGui::TreeNode("Light 0"))
            {
                static bool enableLight = false;
                static float position[] = {0, 0, 0};
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[0].properties.color));
                ImGui::InputFloat3("Position", position);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Light 1"))
            {
                static bool enableLight = false;
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[1].properties.color));
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

        nxs::HighResTimeSource timeSource;
        auto now = timeSource.Now();
        for (int i = 0; i < modelPaths.size(); i++)
        {
            // Preload models
            LoadModel(i);
        }
        LOG_INFO(LogTemp, std::format("Total loading time: {:.3f} seconds", timeSource.Now() - now));
        LoadModel(0);

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
    }

private:
    void InitScene()
    {
        m_scene.CreateNode<nxs::Camera>("Camera");
    }
    void LoadModel(const int index)
    {
        const auto assetPath = GetAssetPath(modelPaths[index]);
        m_model = nxs::Engine::Instance().GetModelManager()->Get(assetPath);
        NXS_ASSERT_MSG(m_model != nullptr, std::format("Failed to load a model file: {}", assetPath));
    }

    void InitLights()
    {
        m_ambient = {0.5, 0.5, 0.5};

        m_directionalLight.properties.color = {1, 1, 1};
        m_directionalLight.direction = {10, -10, 0};

        m_pointLights[0].properties.color = {1, 0, 0};
        m_pointLights[0].position = {5, 0, 0};
        m_pointLights[0].properties.cutoffRange = 100.f;
        m_pointLights[0].constant = 0.01f;

        m_pointLights[1].properties.color = {0, 0, 1};
        m_pointLights[1].position = {-5, 0, 0};
        m_pointLights[1].properties.cutoffRange = 100.f;
        m_pointLights[1].constant = 0.01f;
    }

protected:
    nxs::Scene m_scene;
    nxs::Ref<nxs::GpuProgram> m_gpuProgram;
    //! Currently showing model
    nxs::Ref<nxs::Model> m_model;
    nxs::Transform m_cubeTransform;
    nxs::CameraComponent m_camera;
    nxs::DirectLightComponent m_directionalLight {};
    nxs::PointLightComponent m_pointLights[2] {};
    glm::vec3 m_ambient {0.5, 0.5, 0.5};
    glm::vec3 m_euler {};
    glm::vec3 m_cameraPos {0, 5, 5};
    float m_aoFactor = 1;
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
