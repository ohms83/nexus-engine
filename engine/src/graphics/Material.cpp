//
// Created by nutta on 7/12/2025.
//

#include "graphics/Material.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Material);

// Shader sources
static constexpr auto default_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord0;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    mat4 normalMatrix = transpose(inverse(_Model));
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;
    Normal = normalize(vec3(normalMatrix * vec4(aNormal, 1.0)));

    gl_Position = _Projection * _View * worldPos;
}
)";

static constexpr auto default_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;

struct DirectLight {
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    // Emissive color
    vec3 emissive;
    // Cutoff range
    float cutoff;
    // Direction
    vec3 direction;
};

struct PointLight {
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    // Emissive color
    vec3 emissive;
    // Cutoff range
    float cutoff;
    // Light position
    vec3 position;
    // Constant attenuation
    float constant;
    // Linear attenuation
    float linear;
    // Quadratic attenuation
    float quadratic;
};

uniform vec3 _AmbientLight;
uniform DirectLight _DirectLight;
uniform PointLight _PointLights[20];
uniform int _NumPointLight;

uniform sampler2D _DiffuseMap;
uniform sampler2D _OcclusionMap;
uniform sampler2D _NormalMap;
uniform float _AOFactor;

vec3 CalcAmbientLight()
{
    float ao = texture(_OcclusionMap, TexCoord0).r;
    ao = mix(1.0, ao, _AOFactor);
    return _AmbientLight * ao;
}

vec3 CalcDirLight(DirectLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction); // Direction TO the light source
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(light.position - fragPos);
    if (dist >= light.cutoff) return vec3(0);

    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1 / (light.constant + (light.linear * dist) + (light.quadratic * dist * dist));
    return clamp(light.diffuse * diff * attenuation, 0, 1);
}

void main()
{
    vec3 N = normalize(Normal);
    vec4 albedo = texture(_DiffuseMap, TexCoord0);
    vec3 ambientColor = CalcAmbientLight();
    vec3 directColor = CalcDirLight(_DirectLight, N);
    vec3 pointColor = vec3(0);

    for (int i = 0; i < _NumPointLight; ++i)
    {
        pointColor += CalcPointLight(_PointLights[i], FragPos, N);
    }

    FragColor = albedo * vec4((ambientColor + directColor + pointColor), 1);
}
)";

static const std::map<TextureType, std::string> s_textureTypeUniformNames = {
    {TextureType::Diffuse, "_DiffuseMap"},
    {TextureType::Normal, "_NormalMap"},
    {TextureType::Depth, "_DepthMap"},
    {TextureType::Specular, "_SpecularMap"},
    {TextureType::Emissive, "_EmissiveMap"},
    {TextureType::Ambient, "_OcclusionMap"},
    {TextureType::Opacity, "_OpacityMap"},
    {TextureType::Metalness, "_MetalnessMap"},
    {TextureType::Roughness, "_RoughnessMap"},
    {TextureType::LightMap, "_LightMap"},
};

Material::Material(std::string path, const uint32 resourceId)
    : Resource(std::move(path), resourceId)
{
}

int32 Material::AddTexture(Ref<Texture> texture, TextureType type)
{
    const auto& itr = s_textureTypeUniformNames.find(type);
    if (itr == s_textureTypeUniformNames.end())
    {
        LOG_WARNING(LogMaterial, std::format("Invalid texture type: {}", INT_CAST(type)));
        return -1;
    }

    const int32 slot = INT_CAST(m_textures.size());
    m_textures.emplace_back(texture, itr->second);
    return slot;
}

int32 Material::AddTexture(Ref<Texture> texture, std::string uniform)
{
    const int32 slot = INT_CAST(m_textures.size());
    m_textures.emplace_back(texture, uniform);
    return slot;
}

Ref<Texture> Material::GetTexture(uint32 slot)
{
    if (slot < m_textures.size()) return m_textures[slot].texture;
    LOG_ERROR(LogMaterial, std::format("Invalid slot: {}", slot));
    return nullptr;
}

void Material::SetShader(const Ref<Shader>& shader)
{
    m_shader = shader;
}

void Material::WriteRenderCommand(RenderCommand& command)
{
    if (!m_shader)
    {
        LOG_WARNING(LogMaterial, std::format("Material does not have a shader"));
        return;
    }

    command.uniformVec3.emplace_back("_AmbientColor", ambient);
    command.uniformVec3.emplace_back("_DiffuseColor", diffuse);
    command.uniformVec3.emplace_back("_SpecularColor", specular);
    command.uniformVec3.emplace_back("_EmissiveColor", emissive);
    command.uniformVec3.emplace_back("_Shininess", shininess);

    uint32 slot = 0;
    for (const auto& textureUniformMap : m_textures)
    {
        command.uniform2DTextures.emplace_back(
            textureUniformMap.uniformName,
            slot++,
            textureUniformMap.texture->GetProxy());
    }
}

void Material::CreateDefaultShader(const Ref<RenderingInterface>& renderingInterface)
{
    // TODO: Use shader resource manager
    m_shader.reset(renderingInterface->CreateShader());
    m_shader->BeginCompile()
        .AddSource(default_vertexShaderSource, Shader::Type::Vertex)
        .AddSource(default_fragmentShaderSource, Shader::Type::Fragment)
    .Compile();
}

Ref<Resource> MaterialLoader::Load(const std::string& path, uint32 id)
{
    return nullptr;
}
