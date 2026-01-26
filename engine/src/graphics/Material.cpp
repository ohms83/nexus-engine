//
// Created by nutta on 7/12/2025.
//

#include "graphics/Material.h"
#include "graphics/ShaderManager.h"
#include "nexus/core/serialize/Serializer.h"
#include "nexus/graphics/TextureManager.h"
#include "core/LogDispatcher.h"
#include "core/Path.h"

#include "Remotery.h"

#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(Material);

static constexpr auto default_shader = "shaders/glsl/default_forward_lighting.shader";
static constexpr auto textured_shader = "shaders/glsl/textured_forward_lighting.shader";
static constexpr auto normalmap_shader = "shaders/glsl/normalmap_forward_lighting.shader";

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
    Material::TextureInfo info;
    info.texture = texture;
    info.type = type;
    info.uniformName = itr->second;
    info.path = texture ? texture->GetPath() : std::string();
    m_textures.push_back(std::move(info));
    return slot;
}

int32 Material::AddTexture(Ref<Texture> texture, std::string uniform)
{
    const int32 slot = INT_CAST(m_textures.size());
    Material::TextureInfo info;
    info.texture = texture;
    info.type = TextureType::Undefined;
    info.uniformName = uniform;
    info.path = texture ? texture->GetPath() : std::string();
    m_textures.push_back(std::move(info));
    return slot;
}

Ref<Texture> Material::GetTexture(uint32 slot)
{
    if (slot < m_textures.size()) return m_textures[slot].texture;
    LOG_ERROR(LogMaterial, std::format("Invalid slot: {}", slot));
    return nullptr;
}

void Material::SetShader(Ref<Shader> shader)
{
    m_shader = shader;
}

bool Material::HasTextureType(TextureType type) const
{
    const auto itr = std::ranges::find_if(m_textures, [type](const TextureInfo& texture) {
        return texture.type == type;
    });
    return itr != m_textures.end();
}

Ref<GpuProgram> Material::Use()
{
    if (!m_shader)
    {
        LOG_WARNING(LogMaterial, std::format("Material does not have a shader"));
        return nullptr;
    }

    auto gpuProgram = m_shader->GetGpuProgram();
    NXS_ASSERT(gpuProgram);

    if (!gpuProgram->IsBinding()) gpuProgram->Bind();

    gpuProgram->SetUniformVector("_Material.ambient", ambient);
    gpuProgram->SetUniformVector("_Material.diffuse", diffuse);
    gpuProgram->SetUniformVector("_Material.specular", specular);
    gpuProgram->SetUniformVector("_Material.emissive", emissive);
    gpuProgram->SetUniformFloat("_Material.shininess", shininess);

    uint32 slot = 0;
    for (const auto& textureInfo : m_textures)
    {
        if (!textureInfo.texture) continue;
        gpuProgram->SetUniformTexture2D(
            textureInfo.uniformName,
            textureInfo.texture->GetProxy(),
            slot++);
    }

    return gpuProgram;
}

VariantData Material::Serialize() const
{
    VariantData::Map data;
    data["ambient"] = VariantData::Array{ ambient.r, ambient.g, ambient.b };
    data["diffuse"] = VariantData::Array{ diffuse.r, diffuse.g, diffuse.b };
    data["specular"] = VariantData::Array{ specular.r, specular.g, specular.b };
    data["emissive"] = VariantData::Array{ emissive.r, emissive.g, emissive.b };
    data["shininess"] = DOUBLE_CAST(shininess);
    data["wireframe"] = wireframe;
    data["cull"] = cull;
    data["depthTest"] = depthTest;
    data["depthWrite"] = depthWrite;
    data["blendMode"] = INT_CAST(blendMode);
    data["depthFunction"] = INT_CAST(depthFunction);
    data["shader"] = m_shader ? m_shader->GetPath() : std::string();

    VariantData::Array texArr;
    for (const auto& t : m_textures)
    {
        texArr.emplace_back(VariantData::Map{
            {"path", t.texture ? t.texture->GetPath() : t.path},
            {"type", INT_CAST(t.type)},
            {"uniform", t.uniformName}
        });
    }
    data["textures"] = texArr;

    return data;
}

bool Material::Deserialize(const VariantData& data)
{
    if (data.HasKey("ambient") && data["ambient"].IsArray())
    {
        const auto& arr = data["ambient"].GetArray();
        ambient.r = FLOAT_CAST(arr.at(0).GetDouble());
        ambient.g = FLOAT_CAST(arr.at(1).GetDouble());
        ambient.b = FLOAT_CAST(arr.at(2).GetDouble());
    }
    if (data.HasKey("diffuse") && data["diffuse"].IsArray())
    {
        const auto& arr = data["diffuse"].GetArray();
        diffuse.r = FLOAT_CAST(arr.at(0).GetDouble());
        diffuse.g = FLOAT_CAST(arr.at(1).GetDouble());
        diffuse.b = FLOAT_CAST(arr.at(2).GetDouble());
    }
    if (data.HasKey("specular") && data["specular"].IsArray())
    {
        const auto& arr = data["specular"].GetArray();
        specular.r = FLOAT_CAST(arr.at(0).GetDouble());
        specular.g = FLOAT_CAST(arr.at(1).GetDouble());
        specular.b = FLOAT_CAST(arr.at(2).GetDouble());
    }
    if (data.HasKey("emissive") && data["emissive"].IsArray())
    {
        const auto& arr = data["emissive"].GetArray();
        emissive.r = FLOAT_CAST(arr.at(0).GetDouble());
        emissive.g = FLOAT_CAST(arr.at(1).GetDouble());
        emissive.b = FLOAT_CAST(arr.at(2).GetDouble());
    }
    if (data.HasKey("shininess")) shininess = FLOAT_CAST(data["shininess"].GetDouble());
    if (data.HasKey("wireframe")) wireframe = data["wireframe"].GetBool();
    if (data.HasKey("cull")) cull = data["cull"].GetBool();
    if (data.HasKey("depthTest")) depthTest = data["depthTest"].GetBool();
    if (data.HasKey("depthWrite")) depthWrite = data["depthWrite"].GetBool();
    if (data.HasKey("blendMode")) blendMode = CAST<BlendMode>(data["blendMode"].GetInt());
    if (data.HasKey("depthFunction")) depthFunction = CAST<DepthFunction>(data["depthFunction"].GetInt());

    // Shader path - do not auto-load; store the path and optionally resolve later via Resolve().
    if (data.HasKey("shader"))
    {
        const auto shaderPath = data["shader"].GetString();
        if (!shaderPath.empty())
        {
            m_shader = nullptr; // caller should resolve
            m_shaderPath = shaderPath;
        }
    }

    // Textures
    m_textures.clear();
    if (data.HasKey("textures") && data["textures"].IsArray())
    {
        for (const auto& tv : data["textures"].GetArray())
        {
            const auto& tmap = tv.GetMap();
            TextureInfo ti;
            ti.path = tmap.at("path").GetString();
            ti.type = CAST<TextureType>(tmap.at("type").GetInt());
            ti.uniformName = tmap.at("uniform").GetString();
            ti.texture = nullptr; // The caller should resolve the resource using resource manager
            m_textures.push_back(std::move(ti));
        }
    }

    return true;
}

void Material::Resolve(TextureManager& textureManager, RenderingInterface* renderingInterface)
{
    // Resolve textures using textureManager
    for (auto& ti : m_textures)
    {
        if (!ti.texture && !ti.path.empty())
        {
            auto tex = textureManager.Get<Texture>(ti.path);
            if (tex) ti.texture = tex;
        }
    }

    // Optional: Resolve shader by compiling it if shader path exists.
    if (!m_shader && !m_shaderPath.empty() && renderingInterface)
    {
        // A concrete shader loader or manager would be preferred; temporarily create and compile.
        m_shader = std::make_shared<Shader>(m_shaderPath, 0);
        // Assume m_shaderPath indicates a shader with VS/FS - just attempt compileFromFile if possible.
        // The path format is ambiguous; we do not attempt to auto-compile complex shader objects.
    }
}

std::string Material::GetTexturePath(uint32 slot) const
{
    if (slot < m_textures.size()) return m_textures[slot].path;
    return std::string();
}

std::string Material::GetTextureUniform(uint32 slot) const
{
    if (slot < m_textures.size()) return m_textures[slot].uniformName;
    return std::string();
}

TextureType Material::GetTextureType(uint32 slot) const
{
    if (slot < m_textures.size()) return m_textures[slot].type;
    return TextureType::Undefined;
}

void Material::CreateDefaultShader(Ref<ShaderManager> shaderManager)
{
    std::string veretxShaderSource, fragmentShaderSource, geomtryShaderSource;
    std::string shaderPath;

    if (HasTextureType(TextureType::Normal))
    {
        shaderPath = Path::GetEngineAssetPath(normalmap_shader);
    }
    else if (HasTextureType(TextureType::Diffuse))
    {
        shaderPath = Path::GetEngineAssetPath(textured_shader);
    }
    else
    {
        shaderPath = Path::GetEngineAssetPath(default_shader);
    }

    m_shader = shaderManager->Get<Shader>(shaderPath);
    NXS_ASSERT(m_shader);
}
