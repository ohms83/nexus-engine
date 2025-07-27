//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

DEFINE_LOG(NexusEditor);

#define AXIS_PLUS_X  0x01
#define AXIS_MINUS_X 0x02
#define AXIS_PLUS_Y  0x04
#define AXIS_MINUS_Y 0x08
#define AXIS_PLUS_Z  0x10
#define AXIS_MINUS_Z 0x20

static int axisInput = 0;
static float cameraSpeed = 2.0f;

// Shader sources
static auto vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec3 Normal;
out vec2 texCoord0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    FragPos = vec3(worldPos);
    texCoord0 = aTexCoord0;
    Normal = vec3(model * vec4(aNormal, 1.0));
}
)";

static auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord0;

uniform sampler2D ourTexture;

uniform vec3 u_Ambient;

struct Light {
    // Set w component to 0 for the directional light
    vec3 position;
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    float cutoff;

    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};
uniform Light u_Light;
uniform Light u_PointLights[2];

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

vec3 CalcPointLight(Light light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    if (dist >= light.cutoff) return vec3(0);

    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1 / (light.constantAtt + (light.linearAtt * diff) + (light.quadraticAtt * diff * diff));
    return light.diffuse * diff * attenuation;
}

void main()
{
    vec3 N = normalize(Normal);
    vec4 albedo = texture(ourTexture, texCoord0);
    vec4 ambient = albedo * vec4(u_Ambient, 1);
    vec4 diffuse = albedo * vec4(CalcDirLight(u_Light, N), 1);
    FragColor = ambient + diffuse;
}
)";

static const std::string texturePaths[] = {
    "assets/textures/Wood/Wood052_1K-JPG_Color.jpg",
    "assets/textures/Crate/Wood_Crate_001_basecolor.jpg",
};

static void InitLight(nxs::Scene& scene)
{
    scene.SetAmbient({0.2, 0.2, 0.2});

    {
        auto node = scene.CreateNode<nxs::SceneNode>("Directional Light");
        node->AddComponent<nxs::DirectLightComponent>(nxs::DirectLightComponent {
            {
                nxs::COLOR3F_WHITE,
            },
            glm::vec3(1, -1, 0),
        });
    }
}

int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    constexpr auto resizeable = true;;
    constexpr auto editMode = true;
    constexpr auto maximize = true;
    constexpr nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nxs::RunApplication<NexusEditor>({
        "Nexus Engine",
        graphicsConfig,
        fullscreen,
        resizeable,
        editMode,
        maximize,
    });
}

NexusEditor::~NexusEditor() = default;

bool NexusEditor::Init_Internal()
{
    Application::Init_Internal();

    auto& renderSystem = GetRenderSystem();
    renderSystem.SetClearColor(0x303030ff);

    auto scene = ChangeScene(std::make_shared<nxs::Scene>());
    scene->SetRenderer(std::make_unique<nxs::BasicSceneRenderer>());

    m_camera = scene->CreateNode<nxs::Camera>("Camera Node");
    m_camera->SetPosition({0, 5, 5});
    m_camera->LookAt({0, 0, 0}, {0, 1, 0});
    m_camera->AddComponent<nxs::MoveComponent>(nxs::MoveComponent {
        glm::vec3(0, 0, 0),
        10
    });

    auto& renderInterface = renderSystem.GetRenderInterface();
    m_shader.reset(renderInterface.CreateShader());
    m_shader->BeginCompile()
        .AddSource(vertexShaderSource, nxs::Shader::Type::Vertex)
        .AddSource(fragmentShaderSource, nxs::Shader::Type::Fragment)
    .Compile();

    {
        auto texture = GetTextureManager().Get(texturePaths[0]);
        texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);
        texture->AllocateGpuResource(renderInterface);

        auto mesh = GetMeshManager().GetStaticMesh(nxs::Mesh::PlaneMesh);
        auto node = scene->CreateNode<nxs::SceneNode>("Plane Node");
        nxs::RenderComponent renderComponent = {
            mesh->GetVertexBuffer(),
            mesh->GetIndexBuffer(),
            m_shader.get()
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
        auto texture = GetTextureManager().Get(texturePaths[1]);
        texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);
        texture->AllocateGpuResource(renderInterface);

        auto mesh = GetMeshManager().GetStaticMesh(nxs::Mesh::CubeMesh);
        auto node = scene->CreateNode<nxs::SceneNode>("Cube Node");
        nxs::RenderComponent renderComponent = {
            mesh->GetVertexBuffer(),
            mesh->GetIndexBuffer(),
            m_shader.get()
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

    InitLight(*scene);
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

    if (key == SDLK_S) axisInput |= AXIS_PLUS_Y;
    if (key == SDLK_W) axisInput |= AXIS_MINUS_Y;
    if (key == SDLK_A) axisInput |= AXIS_MINUS_X;
    if (key == SDLK_D) axisInput |= AXIS_PLUS_X;
    if (key == SDLK_Q) axisInput |= AXIS_PLUS_Z;
    if (key == SDLK_E) axisInput |= AXIS_MINUS_Z;
}

void NexusEditor::OnKeyUp(const SDL_Keycode key)
{
    Application::OnKeyUp(key);

    if (key == SDLK_S) axisInput &= ~AXIS_PLUS_Y;
    if (key == SDLK_W) axisInput &= ~AXIS_MINUS_Y;
    if (key == SDLK_A) axisInput &= ~AXIS_MINUS_X;
    if (key == SDLK_D) axisInput &= ~AXIS_PLUS_X;
    if (key == SDLK_Q) axisInput &= ~AXIS_PLUS_Z;
    if (key == SDLK_E) axisInput &= ~AXIS_MINUS_Z;
}

void NexusEditor::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    Application::OnResize(screenSize, actualSize);
    m_camera->SetProjection(45.f, CAST<float>(actualSize.x), CAST<float>(actualSize.y), 0.1f, 100.f);
}

void NexusEditor::Update()
{
    Application::Update();

    auto camera = GetCurrentScene()->GetNode("Camera Node");
    if (!camera) return;

    glm::vec3 translation{};
    if (axisInput & AXIS_PLUS_X)  translation.x += 1.0f;
    if (axisInput & AXIS_MINUS_X) translation.x -= 1.0f;
    if (axisInput & AXIS_PLUS_Y)  translation.z += 1.0f;
    if (axisInput & AXIS_MINUS_Y) translation.z -= 1.0f;
    if (axisInput & AXIS_PLUS_Z)  translation.y += 1.0f;
    if (axisInput & AXIS_MINUS_Z) translation.y -= 1.0f;

    // Transform the translation vector into the camera's local coordinate.
    translation = camera->GetRotation() * translation;
    camera->Translate(translation * cameraSpeed * GetDeltaTime());
}
