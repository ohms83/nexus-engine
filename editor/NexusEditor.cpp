//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

#include <sstream>

#include "nexus/Engine.h"
#include "nexus/graphics/Model.h"

DEFINE_LOG(NexusEditor);

static float cameraSpeed = 2.0f;

static nxs::Ref<nxs::Camera> InitCamera(nxs::Scene& scene)
{
    auto camera = scene.CreateNode<nxs::Camera>("Camera Node");
    camera->Position().value = {0, 5, 5};
    camera->LookAt({0, 0, 0}, {0, 1, 0});
    camera->AddComponent<nxs::MoveComponent>(nxs::MoveComponent {
        glm::vec3(0, 0, 0),
        10
    });
    return camera;
}

static void InitLight(nxs::Scene& scene)
{
    scene.Ambient() = {0.5, 0.2, 0.2};

    {
        auto light = scene.CreateNode<nxs::DirectionalLight>("Direct Light 1");
        light->Properties().color =  {1, 1, 1};
        light->Direction() = {10, -10, 0};
    }
    {
        auto light = scene.CreateNode<nxs::PointLight>("Point Light 1");
        light->Position() = {5, 0, 0};
        light->PointLightProperties().constant = 0.01f;
        
        auto& properties = light->Properties();
        properties.color = {1, 0, 0};
        properties.cutoffRange = 100.f;
    }
    {
        auto light = scene.CreateNode<nxs::PointLight>("Point Light 2");
        light->Position() = {-5, 0, 0};
        light->PointLightProperties().constant = 0.01f;
        
        auto& properties = light->Properties();
        properties.color = {0, 1, 0};
        properties.cutoffRange = 100.f;
    }
}

int main()
{
    const std::filesystem::path configFile = std::filesystem::path(NXS_ASSETS_DIR) / "config/editorConfig.ini";

    nxs::ApplicationConfig config;
    config.LoadConfig(configFile.string());
#if 0
    config.allowProfile = false;
#endif
    return nxs::RunApplication<NexusEditor>(config);
}

NexusEditor::~NexusEditor()
{
}

void NexusEditor::InitModel()
{
    const auto engine = nxs::Engine::Instance();
    const auto taskScheduler = engine.GetTaskScheduler();
    const auto resourceManager = engine.GetModelManager();
    const auto assetPath = GetAssetPath("meshes/buddha/buddha.obj");
    auto loadResult = resourceManager->RequestResourceAsync(assetPath, *taskScheduler);
    auto waitingTask = std::make_shared<nxs::IntervalTask>(0, [this, loadResult]() {
        const auto status = loadResult->status;
        if (status == nxs::IResourceLoader::LoadResult::Status::Ready)
        {
            LOG_INFO(LogTemp, std::format("Finished loading model {}", loadResult->path));

            auto scene = nxs::Engine::Instance().GetSceneManager()->GetCurrentScene();
            auto node = scene->CreateNode<nxs::SceneNode3D>("Model");
            auto model = PTR_CAST<nxs::Model>(loadResult->resource);
            node->AddComponent<nxs::ModelComponent>().model = model;
            node->Position().Translate(-model->GetBoundingSphere().center * node->Scale().value);
            return false;
        }
        else if (status == nxs::IResourceLoader::LoadResult::Status::Failed)
        {
            LOG_INFO(LogTemp, std::format("Error loading model {}!!", loadResult->path));
            return false;
        }
        return true;
    });
    taskScheduler->ScheduleTask(waitingTask);
}

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

    auto& engine = nxs::Engine::Instance();

    auto& renderSystem = GetRenderSystem();
    renderSystem.SetClearColor(0x303030ff);

    auto sceneManager = engine.GetSceneManager();
    auto scene = sceneManager->EmplaceAndChange<nxs::Scene>("Editor Scene");
    scene->SetRenderer(std::make_unique<nxs::BasicSceneRenderer>());

    m_camera = InitCamera(*scene);
    InitModel();
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

    m_editor->AddWidget(std::make_shared<nxs::SceneGraphWidget>(scene));
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

    auto& inputManager = nxs::InputManager::Instance();

    glm::vec3 translation = inputManager.GetAxisValue("movement");

    // Transform the translation vector into the camera's local coordinate.
    auto& cameraOrient = m_camera->Orient();
    auto& cameraPosition = m_camera->Position();
    translation = cameraOrient.value * translation;
    cameraPosition.Translate(translation * cameraSpeed * GetDeltaTime());

    glm::vec2 euler = inputManager.GetMouseAxisValue("camera_turn") * GetDeltaTime();
    glm::vec2 keyDeltaEuler = inputManager.GetAxisValue("camera_turn") * 20.f * GetDeltaTime();
    cameraOrient.Rotate(glm::vec3(euler.y + keyDeltaEuler.y, euler.x + keyDeltaEuler.x, 0));
}
