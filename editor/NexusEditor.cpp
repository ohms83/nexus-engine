//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

#include <sstream>

#include "nexus/Engine.h"
#include "nexus/graphics/Model.h"

static nxs::Ptr<nxs::Mesh> g_cubeMesh;
static nxs::Ptr<nxs::Mesh> g_planeMesh;

DEFINE_LOG(NexusEditor);

static float cameraSpeed = 2.0f;

static const std::string texturePaths[] = {
    "textures/Wood/Wood052_1K-JPG_Color.jpg",
    "textures/Crate/Wood_Crate_001_basecolor.jpg",
};

static constexpr auto vertexShaderPath = "shader/forward_ligthing.vs";
static constexpr auto fragmentShaderPath = "shader/forward_ligthing.fs";

static constexpr auto appleModel = "meshes/apple/3DApple001_SQ-1K-PNG.obj";
static constexpr auto barrelModel = "meshes/barrel/wine_barrel_01_4k.gltf";

static void InitLight(nxs::Scene& scene)
{
    scene.Ambient() = {0.2, 0.2, 0.2};

    {
        auto node = scene.CreateNode<nxs::SceneNode>("Directional Light");
        node->AddComponent<nxs::DirectLightComponent>(nxs::DirectLightComponent {
            {
                nxs::Color3F::Grey,
            },
            glm::vec3(-1, -1, 0),
        });
    }

    {
        auto node = scene.CreateNode<nxs::SceneNode>("Point Light 01");
        node->AddComponent<nxs::PointLightComponent>(nxs::PointLightComponent {
            {
                nxs::Color3F::Yellow, 1, 1, 10.f
            },
            2.0f,
            0.5f,
            0.05f,
        });
    }

    {
        auto node = scene.CreateNode<nxs::SceneNode>("Point Light 02");
        node->AddComponent<nxs::PointLightComponent>(nxs::PointLightComponent {
            {
                nxs::Color3F::Blue, 1, 1, 10.f
            },
            2.0f,
            0.5f,
            0.05f,
        });
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
    g_cubeMesh.reset();
    g_planeMesh.reset();
}

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

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

    auto renderInterface = renderSystem.GetRenderInterface();

    std::fstream vertexShader(GetAssetPath(vertexShaderPath), std::ios::in);
    std::fstream fragmentShader(GetAssetPath(fragmentShaderPath), std::ios::in);
    if (vertexShader.bad() || fragmentShader.bad())
    {
        LOG_FATAL(LogNexusEditor,
            std::format("Failed to load shaders! VS={} FS={}", vertexShaderPath, fragmentShaderPath));
    }
    std::stringstream vertexShaderStream;
    std::stringstream fragmentShaderStream;

    vertexShaderStream << vertexShader.rdbuf();
    fragmentShaderStream << fragmentShader.rdbuf();

    m_gpuProgram.reset(renderInterface->CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertexShaderStream.str(), nxs::GpuProgram::Type::Vertex)
        .AddSource(fragmentShaderStream.str(), nxs::GpuProgram::Type::Fragment)
    .Compile();

    const auto modelPath = std::filesystem::path(NXS_ASSETS_DIR) / "meshes/apple/3DApple001_SQ-1K-PNG.obj";
    const auto modelManager = nxs::Engine::Instance().GetModelManager();
    m_model = modelManager->Get(modelPath.string());

    auto& engine = nxs::Engine::Instance();

    {
        const auto texturePath = GetAssetPath(texturePaths[0]);
        auto texture = engine.GetTextureManager()->Get(texturePath);
        texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

        g_planeMesh = std::make_unique<nxs::PlaneMesh>(renderInterface);
        auto node = scene->CreateNode<nxs::SceneNode>("Plane Node");
        nxs::RenderComponent renderComponent = {
            g_planeMesh->GetVertexBuffer(),
            g_planeMesh->GetIndexBuffer(),
            m_gpuProgram
        };
        node->AddComponent<nxs::DiffuseMapComponent>(nxs::DiffuseMapComponent {
            {texture->GetProxy()}
        });
        node->AddComponent<nxs::RenderComponent>(renderComponent);
        node->AddComponent<nxs::TransformComponent>(nxs::TransformComponent {
            glm::vec3(0, 0, 0),
            glm::quat(1, 0, 0, 0),
            glm::vec3(5, 5, 5),
        });
    }

    {
        const auto texturePath = GetAssetPath(texturePaths[1]);
        auto texture = engine.GetTextureManager()->Get(texturePath);
        texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

        m_model = engine.GetModelManager()->Get(GetAssetPath(barrelModel));
        auto mesh = m_model->GetMeshes()[2];
        g_cubeMesh = std::make_unique<nxs::CubeMesh>(renderInterface);
        auto node = scene->CreateNode<nxs::SceneNode>("Cube Node");
        nxs::RenderComponent renderComponent = {
            mesh->GetVertexBuffer(),
            mesh->GetIndexBuffer(),
            m_gpuProgram
        };
        node->AddComponent<nxs::DiffuseMapComponent>(nxs::DiffuseMapComponent {
            // {texture->GetProxy()}
            {mesh->GetMaterial()->GetTexture(0)->GetProxy()},
        });
        node->AddComponent<nxs::RenderComponent>(renderComponent);
        node->AddComponent<nxs::TransformComponent>(nxs::TransformComponent {
            {0, 1, 0},
            glm::quat(1, 0, 0, 0),
            {2, 2, 2}
        });
        node->AddComponent<nxs::RotationComponent>(nxs::RotationComponent {
            glm::normalize(glm::sphericalRand<float>(1)),
            90.f
        });
    }

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

void NexusEditor::InitCube(nxs::Scene& scene, const nxs::int32 row, const nxs::int32 col)
{
    const auto& renderInterface = GetRenderSystem().GetRenderInterface();
    constexpr float gridWidth  = 5;
    constexpr float gridHeight = 5;

    float x = (col - 1) * gridWidth / 2;
    float z = 0;

    auto& engine = nxs::Engine::Instance();
    const auto texturePath = GetAssetPath(texturePaths[1]);
    const auto texture = engine.GetTextureManager()->Get(texturePath);
    texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
    texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

    g_cubeMesh = std::make_unique<nxs::CubeMesh>(renderInterface);
    const auto node = scene.CreateNode<nxs::SceneNode>("Cube Node");
    nxs::RenderComponent renderComponent = {
        g_cubeMesh->GetVertexBuffer(),
        g_cubeMesh->GetIndexBuffer(),
        m_gpuProgram
    };
    node->AddComponent<nxs::DiffuseMapComponent>(nxs::DiffuseMapComponent {
        {texture->GetProxy()}
    });
    node->AddComponent<nxs::RenderComponent>(renderComponent);
    node->AddComponent<nxs::TransformComponent>(nxs::TransformComponent {
        {0, 1, 0},
        glm::quat(1, 0, 0, 0),
        {1, 1, 1}
    });
    node->AddComponent<nxs::RotationComponent>(nxs::RotationComponent {
        glm::normalize(glm::sphericalRand<float>(1)),
        90.f
    });
}
