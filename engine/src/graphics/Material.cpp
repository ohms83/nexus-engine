//
// Created by nutta on 7/12/2025.
//

#include "graphics/Material.h"
#include "core/LogDispatcher.h"
#include "core/Path.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Material);

static constexpr auto default_vertex_shader = "shaders/default_forward_lighting.vert";
static constexpr auto default_fragment_shader = "shaders/default_forward_lighting.frag";

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

    command.uniformVec3.emplace_back("_Material.ambient", ambient);
    command.uniformVec3.emplace_back("_Material.diffuse", diffuse);
    command.uniformVec3.emplace_back("_Material.specular", specular);
    command.uniformVec3.emplace_back("_Material.emissive", emissive);
    command.uniformFloats.emplace_back("_Material.shininess", shininess);

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
    // TODO: Use resource manager.
    const auto vertexShaderPath = Path::GetEngineAssetPath(default_vertex_shader);
    const auto fragmentShaderPath = Path::GetEngineAssetPath(default_fragment_shader);
    std::fstream vertexShader(vertexShaderPath, std::ios::in);
    std::fstream fragmentShader(fragmentShaderPath, std::ios::in);
    if (!vertexShader.is_open() || !fragmentShader.is_open())
    {
        LOG_FATAL(LogMaterial,
            std::format("Failed to load shaders! VS={} FS={}", vertexShaderPath, fragmentShaderPath));
        return;
    }

    std::stringstream vertexShaderStream, fragmentShaderStream;
    vertexShaderStream << vertexShader.rdbuf();
    fragmentShaderStream << fragmentShader.rdbuf();

    // TODO: Use shader resource manager
    m_shader.reset(renderingInterface->CreateShader());
    m_shader->BeginCompile()
        .AddSource(vertexShaderStream.str(), Shader::Type::Vertex)
        .AddSource(fragmentShaderStream.str(), Shader::Type::Fragment)
    .Compile();
}

Ref<Resource> MaterialLoader::Load(const std::string& path, uint32 id)
{
    return nullptr;
}
