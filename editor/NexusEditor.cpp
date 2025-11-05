//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

#include <sstream>

#include "nexus/Engine.h"
#include "nexus/graphics/Model.h"
#include "nexus/editor/widget/PropertyWindow.h"

DEFINE_LOG(NexusEditor);

static float cameraSpeed = 2.0f;
static nxs::SceneNode::Id selectedNode = nxs::SceneNode::InvalidID;

static nxs::Ref<nxs::Camera> InitCamera(nxs::Scene& scene)
{
    auto camera = scene.EmplaceChild<nxs::Camera>("Camera Node");
    camera->Position().value = {0, 50, 0};
    camera->LookAt({0, 50, -10}, {0, 1, 0});
    // camera->Position().value = {0, 0, 5};
    // camera->LookAt({0, 0, 0}, {0, 1, 0});
    camera->Properties().farZ = 10000.f;
    camera->AddComponent<nxs::MoveComponent>(nxs::MoveComponent {
        glm::vec3(0, 0, 0),
        100
    });
    return camera;
}

static void InitLight(nxs::Scene& scene)
{
    scene.Ambient() = {0.5, 0.5, 0.5};

    {
        auto light = scene.EmplaceChild<nxs::DirectionalLight>("Direct Light 1");
        light->Properties().color =  {1, 1, 1};
        light->Direction() = {10, -10, 0};
    }
    {
        auto light = scene.EmplaceChild<nxs::PointLight>("Red Light");
        light->Position() = {50, 50, 0};

        auto& pointLight = light->PointLightProperties();
        pointLight.constant = 0.01f;
        pointLight.linear = 0.0005f;
        pointLight.quadratic = 0.00001f;

        auto& properties = light->Properties();
        properties.color = {1, 0, 0};
        properties.cutoffRange = 1000.f;
    }
    {
        auto light = scene.EmplaceChild<nxs::PointLight>("Green Light");
        light->Position() = {-50, 50, 0};

        auto& pointLight = light->PointLightProperties();
        pointLight.constant = 0.01f;
        pointLight.linear = 0.0005f;
        pointLight.quadratic = 0.00001f;

        auto& properties = light->Properties();
        properties.color = {0, 1, 0};
        properties.cutoffRange = 1000.f;
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
    const auto assetPath = GetAssetPath("meshes/sponza/sponza.obj");
    // const auto assetPath = GetAssetPath("meshes/barrel/wine_barrel_01_4k.gltf");
    auto loadResult = resourceManager->RequestResourceAsync(assetPath, *taskScheduler);
    auto waitingTask = std::make_shared<nxs::IntervalTask>(0, [this, loadResult]() {
        const auto status = loadResult->status;
        if (status == nxs::IResourceLoader::LoadResult::Status::Ready)
        {
            LOG_INFO(LogTemp, std::format("Finished loading model {}", loadResult->path));

            auto scene = nxs::Engine::Instance().GetSceneManager()->GetCurrentScene();
            auto model = PTR_CAST<nxs::Model>(loadResult->resource);
            auto path = std::filesystem::path(model->GetPath());
            auto node = scene->EmplaceChild<nxs::ModelNode>(path.filename().string());
            node->SetModel(model);
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
    scene->SetRenderer(std::make_unique<nxs::BasicSceneRenderer>(GetRenderSystem()));

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

    m_sceneGraphWidget = std::make_shared<nxs::SceneGraphWidget>(*sceneManager);
    m_propertyWindow = std::make_shared<nxs::PropertyWindow>(*sceneManager);

    const auto toolsMenuName = "Tools";
    auto& menu = m_editor->GetMenu();

    menu.AddMenuItem(toolsMenuName,
        std::make_shared<nxs::WidgetMenuItem> (
            0,
            "Scene Graph",
            "",
            "",
            m_sceneGraphWidget,
            CAST<nxs::IWidgetOwner&>(*m_editor.get())
        )
    );
    menu.AddMenuItem(toolsMenuName,
        std::make_shared<nxs::WidgetMenuItem> (
            0,
            "Property Window",
            "",
            "",
            m_propertyWindow,
            CAST<nxs::IWidgetOwner&>(*m_editor.get())
        )
    );
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
    auto& properties = m_camera->Properties();
    m_camera->SetProjection(
        45.f,
        CAST<float>(actualSize.x),
        CAST<float>(actualSize.y),
        properties.nearZ,
        properties.farZ);
}

void NexusEditor::Update()
{
    Application::Update();

    const auto& inputManager = nxs::InputManager::Instance();

    glm::vec3 translation = inputManager.GetAxisValue("movement");

    // Transform the translation vector into the camera's local coordinate.
    auto& cameraOrient = m_camera->Orient();
    auto& cameraPosition = m_camera->Position();
    translation = cameraOrient.quat * translation;
    cameraPosition.Translate(translation * cameraSpeed * GetDeltaTime());

    glm::vec2 euler = inputManager.GetMouseAxisValue("camera_turn") * GetDeltaTime();
    glm::vec2 keyDeltaEuler = inputManager.GetAxisValue("camera_turn") * 20.f * GetDeltaTime();
    cameraOrient.Rotate(glm::vec3(euler.y + keyDeltaEuler.y, euler.x + keyDeltaEuler.x, 0));

    const auto selectedNode = m_sceneGraphWidget->GetSelectedNode();
    m_propertyWindow->SetSceneNode(selectedNode);
}
