//
// Created by nutta on 7/18/2025.
//

#include "NexusEditor.h"

DEFINE_LOG(NexusEditor);

static nxs::Color3F ambient {0.5, 0.5, 0.5};
static nxs::DirectionalLight directionalLight {};
static nxs::PointLight pointLights[2] {};

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
    vec3 lightDir = normalize(light.position);
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
    vec4 albedo = texture(ourTexture, texCoord0);
    vec4 ambient = albedo * vec4(u_Ambient, 1);
    vec4 diffuse = albedo * vec4(CalcDirLight(u_Light, Normal), 1);
    FragColor = ambient + diffuse;
    // FragColor = albedo;
}
)";

static const std::string assetsPath = "assets/textures/Crate/Wood_Crate_001_basecolor.jpg";

static void InitLight()
{
    ambient = {0.5, 0.5, 0.5};

    directionalLight.diffuseColor = {1, 1, 1};
    directionalLight.transform.SetPosition({10, 10, 0});

    pointLights[0].diffuseColor = {0.5, 0, 0};
    pointLights[0].transform.SetPosition({10, 10, 0});

    pointLights[1].diffuseColor = {0, 0.5, 0};
    pointLights[1].transform.SetPosition({-10, 10, 0});
}

int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    constexpr auto resizeable = true;;
    constexpr auto editMode = true;
    constexpr auto maximize = true;
    const nxs::GraphicsConfig graphicsConfig {
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

    m_camera.transform.LookAt({0, 3, 3}, {0, 0, 0}, {0, 1, 0});
    m_planeMesh = GetMeshManager().GetStaticMesh(nxs::Mesh::PlaneMesh);
    // m_planeMesh = GetMeshManager().GetStaticMesh(nxs::Mesh::CubeMesh);

    InitLight();

    auto& renderInterface = renderSystem.GetRenderInterface();
    m_shader.reset(renderInterface.CreateShader());
    m_shader->BeginCompile()
        .AddSource(vertexShaderSource, nxs::Shader::Type::Vertex)
        .AddSource(fragmentShaderSource, nxs::Shader::Type::Fragment)
    .Compile();

    m_texture = GetTextureManager().Get(assetsPath);
    m_texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
    m_texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);
    m_texture->AllocateGpuResource(renderInterface);

    return true;
}

void NexusEditor::OnEvent(const SDL_Event& e)
{
    Application::OnEvent(e);
}

void NexusEditor::Render(nxs::RenderSystem& renderSystem)
{
    Application::Render(renderSystem);
    // Calculate matrices for a rotating cube
    const auto dt = GetDeltaTime();
    // m_cubeTransform.Rotate(90.f * dt, glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), // Camera position
                                 glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                                 glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
                                );

    const nxs::RenderCommand renderCommand
    {
        m_shader.get(),
        m_planeMesh->GetVertexBuffer(),
        m_planeMesh->GetIndexBuffer(),
        {
            {"model", glm::identity<glm::mat4>()},
            // {"view", m_camera.GetViewMtx()},
            {"view", view},
            {"projection", m_camera.GetProjectionMtx()},
        },
        {
            { "ourTexture", 0, m_texture->GetProxy() },
        },
        {
            { "u_Ambient", ambient },
            { "u_Light.position", glm::vec3(1, 1, 0) },
            { "u_Light.diffuse", directionalLight.diffuseColor    },
        }
    };

    renderSystem.RegisterDrawCommand(renderCommand);
}

void NexusEditor::OnKeyDown(const SDL_Keycode key)
{
    Application::OnKeyDown(key);
}

void NexusEditor::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    Application::OnResize(screenSize, actualSize);
    m_camera.SetProjection(45.f, CAST<float>(actualSize.x), CAST<float>(actualSize.y), 0.1f, 100.f);
}
