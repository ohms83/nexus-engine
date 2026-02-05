//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

#include <sstream>

#include "nexus/Engine.h"
#include "nexus/graphics/Model.h"
#include "nexus/editor/widget/PropertyWindow.h"

#include "utils/CameraUtils.h"

#define LOAD_ASYNC 1

DEFINE_LOG(NexusEditor);

static nxs::Identifier selectedNode = nxs::InvalidID;

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
    const auto resourceManager = engine.GetResourceManager();
    const auto assetPath = GetAssetPath("meshes/sponza/sponza.obj");
    // const auto assetPath = GetAssetPath("meshes/barrel/wine_barrel_01_4k.gltf");
#if LOAD_ASYNC
    auto loadResult = resourceManager->GetResourceAsync(typeid(nxs::Model), assetPath, *taskScheduler);
    auto waitingTask = std::make_shared<nxs::IntervalTask>(0, [this, loadResult]() {
        const auto status = loadResult->status;
        if (status == nxs::IResourceLoader::LoadResult::Status::Ready)
        {
            LOG_INFO(LogTemp, std::format("Finished loading model {}", loadResult->path));

            auto scene = nxs::Engine::Instance().GetSceneManager()->GetCurrentScene();
            auto model = PTR_CAST<nxs::Model>(loadResult->resource);
            auto filename = std::filesystem::path(model->GetPath()).filename();
            auto modelNode = scene->EmplaceChild<nxs::ModelNode>(filename.string());
            NXS_ASSERT(PTR_CAST<nxs::SceneNode>(modelNode));
            modelNode->SetModel(model);
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
#else
    auto scene = nxs::Engine::Instance().GetSceneManager()->GetCurrentScene();
    auto model = resourceManager->Get<nxs::Model>(assetPath);
    auto filename = std::filesystem::path(model->GetPath()).filename();
    auto modelNode = scene->EmplaceChild<nxs::ModelNode>(filename.string());
    NXS_ASSERT(PTR_CAST<nxs::SceneNode>(modelNode));
    modelNode->SetModel(model);
#endif
}

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

    auto& engine = nxs::Engine::Instance();

    auto& renderSystem = GetRenderSystem();
    renderSystem.SetClearColor(0x303030ff);

    auto sceneManager = engine.GetSceneManager();
    auto scene = sceneManager->EmplaceAndChange<nxs::Scene>("Editor Scene");
    scene->SetRenderer(std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem()));

    InitModel();
    InitLight(*scene);

    m_camera = nxs::editor::CameraUtils::InitCamera(*scene);

    m_sceneGraphWidget = std::make_shared<nxs::SceneGraphWidget>(*sceneManager);
    m_propertyWindow = std::make_shared<nxs::PropertyWindow>(*sceneManager);

    const auto windowMenuName = "Window";
    auto& menu = m_editor->GetMenu();

    menu.AddMenuItem(windowMenuName,
        std::make_shared<nxs::WidgetMenuItem> (
            0,
            "Scene Graph",
            "",
            "",
            m_sceneGraphWidget,
            CAST<nxs::IWidgetOwner&>(*m_editor.get())
        )
    );
    menu.AddMenuItem(windowMenuName,
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

    const auto selectedNode = m_sceneGraphWidget->GetSelectedNode();
    m_propertyWindow->SetSceneNode(selectedNode);
}
