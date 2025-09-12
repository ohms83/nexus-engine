//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

#include <sstream>

#include "nexus/Engine.h"
#include "nexus/graphics/Model.h"

DEFINE_LOG(NexusEditor);

static float cameraSpeed = 2.0f;

static void InitLight(nxs::Scene& scene)
{
    scene.Ambient() = {0.5, 0.2, 0.2};

    {
        auto light = scene.CreateNode<nxs::DirectionalLight>("Direct Light 1");
        light->SetColor({1, 1, 1});
        light->GetLightComponent().direction = {10, -10, 0};
    }
    {
        auto light = scene.CreateNode<nxs::PointLight>("Point Light 1");
        light->SetColor({1, 0, 0});
        light->Position() = {5, 0, 0};

        auto& component = light->GetLightComponent();
        // component.position = {5, 0, 0};
        component.properties.cutoffRange = 100.f;
        component.constant = 0.01f;
    }
    {
        auto light = scene.CreateNode<nxs::PointLight>("Point Light 2");
        light->SetColor({0, 0, 1});
        light->Position() = {-5, 0, 0};

        auto& component = light->GetLightComponent();
        // component.position = {-5, 0, 0};
        component.properties.cutoffRange = 100.f;
        component.constant = 0.01f;
    }
}

int main()
{
    const std::filesystem::path configFile = std::filesystem::path(NXS_ASSETS_DIR) / "config/editorConfig.ini";

    nxs::ApplicationConfig config;
    config.LoadConfig(configFile.string());
    return nxs::RunApplication<NexusEditor>(config);
}

NexusEditor::~NexusEditor()
{
}

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

    auto& engine = nxs::Engine::Instance();

    auto& renderSystem = GetRenderSystem();
    renderSystem.SetClearColor(0x303030ff);

    auto scene = ChangeScene(std::make_shared<nxs::Scene>());
    scene->SetRenderer(std::make_unique<nxs::BasicSceneRenderer>());

    m_camera = scene->CreateNode<nxs::Camera>("Camera Node");
    m_camera->Position().value = {0, 5, 5};
    m_camera->LookAt({0, 0, 0}, {0, 1, 0});
    m_camera->AddComponent<nxs::MoveComponent>(nxs::MoveComponent {
        glm::vec3(0, 0, 0),
        10
    });

    auto node = scene->CreateNode<nxs::SceneNode3D>("Model");
    node->Scale().value = glm::vec3 {3, 3, 3};

    const auto modelPath = std::filesystem::path(NXS_ASSETS_DIR) / "meshes/apple/3DApple001_SQ-1K-PNG.obj";
    const auto modelManager = engine.GetModelManager();
    node->AddComponent<nxs::ModelComponent>().model = modelManager->Get(modelPath.string());

    InitLight(*scene);

    auto& inputManager = nxs::InputManager::Instance();
    nxs::KeyInputMap cameraMovementKeyInput = {
        {
            {SDLK_W, nxs::KeyInputMap::AxisMinusZ},
            {SDLK_S, nxs::KeyInputMap::AxisPlusZ},
            {SDLK_A, nxs::KeyInputMap::AxisMinusX},
            {SDLK_D, nxs::KeyInputMap::AxisPlusX},
            {SDLK_Q, nxs::KeyInputMap::AxisMinusY},
            {SDLK_E, nxs::KeyInputMap::AxisPlusY},
        }
    };
    nxs::KeyInputMap cameraTurnKeyInput = {
        {
            {SDLK_LEFT, nxs::KeyInputMap::AxisPlusX},
            {SDLK_RIGHT, nxs::KeyInputMap::AxisMinusX},
            {SDLK_UP, nxs::KeyInputMap::AxisPlusY},
            {SDLK_DOWN, nxs::KeyInputMap::AxisMinusY},
        }
    };
    nxs::MouseAxisMapping cameraTurnMouseInput = {
        true,
        SDL_BUTTON_RIGHT,
        {5, 5}
    };
    inputManager.RegisterAxisInputMap("movement", cameraMovementKeyInput);
    inputManager.RegisterAxisInputMap("camera_turn", cameraTurnKeyInput);
    inputManager.RegisterMouseAxisInputMap("camera_turn", cameraTurnMouseInput);
    return true;
}

void NexusEditor::OnEvent(const SDL_Event& e)
{
    Application::OnEvent(e);
}

void NexusEditor::Render(nxs::RenderSystem& renderSystem)
{
    Application::Render(renderSystem);
}

void NexusEditor::OnKeyDown(const SDL_Keycode key)
{
    Application::OnKeyDown(key);
}

void NexusEditor::OnKeyUp(const SDL_Keycode key)
{
    Application::OnKeyUp(key);
}

void NexusEditor::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    Application::OnResize(screenSize, actualSize);
    m_camera->SetProjection(45.f, CAST<float>(actualSize.x), CAST<float>(actualSize.y), 0.1f, 100.f);
}

void NexusEditor::Update()
{
    Application::Update();

    const auto camera = PTR_CAST<nxs::Camera>(GetCurrentScene()->GetNode("Camera Node"));
    if (!camera) return;

    auto& inputManager = nxs::InputManager::Instance();

    glm::vec3 translation = inputManager.GetAxisValue("movement");

    // Transform the translation vector into the camera's local coordinate.
    auto& cameraOrient = camera->Orient();
    auto& cameraPosition = camera->Position();
    translation = cameraOrient.value * translation;
    cameraPosition.Translate(translation * cameraSpeed * GetDeltaTime());

    glm::vec2 euler = inputManager.GetMouseAxisValue("camera_turn") * GetDeltaTime();
    glm::vec2 keyDeltaEuler = inputManager.GetAxisValue("camera_turn") * 20.f * GetDeltaTime();
    cameraOrient.Rotate(glm::vec3(euler.y + keyDeltaEuler.y, euler.x + keyDeltaEuler.x, 0));
}
