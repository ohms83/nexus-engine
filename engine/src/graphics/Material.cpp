//
// Created by nutta on 7/12/2025.
//

#include "graphics/Material.h"

USING_NAMESPACE_NXS;

const std::string Material::AmbientColorUniform = "uMaterial.ambientColor";
const std::string Material::DiffuseColorUniform = "uMaterial.diffuseColor";
const std::string Material::SpecularColorUniform = "uMaterial.specularColor";
const std::string Material::EmissiveColorUniform = "uMaterial.emissiveColor";
const std::string Material::ShininessUniform = "uMaterial.shininess";
const std::string Material::BlendModeUniform = "uMaterial.blendMode";
const std::string Material::DiffuseMapUniform = "uMaterial.diffuseMap";
const std::string Material::SpecularMapUniform = "uMaterial.specularMap";
const std::string Material::EmissiveMapUniform = "uMaterial.emissiveMap";
const std::string Material::NormalMapUniform = "uMaterial.normalMap";
const std::string Material::DepthMapUniform = "uMaterial.depthMap";

Material::Material(std::string path, const uint32 resourceId)
    : Resource(std::move(path), resourceId)
{
}

void Material::SetDiffuseTexture(const Ref<Texture>& texture, const uint32 slot)
{
    SetPredefinedTexture(texture, DiffuseMapUniform, slot);
}

void Material::SetSpecularTexture(const Ref<Texture>& texture, const uint32 slot)
{
    SetPredefinedTexture(texture, SpecularMapUniform, slot);
}

void Material::SetEmissiveTexture(const Ref<Texture>& texture, const uint32 slot)
{
    SetPredefinedTexture(texture, EmissiveMapUniform, slot);
}

void Material::SetNormalTexture(const Ref<Texture>& texture, const uint32 slot)
{
    SetPredefinedTexture(texture, NormalMapUniform, slot);
}

void Material::SetHeightTexture(const Ref<Texture>& texture, const uint32 slot)
{
    SetPredefinedTexture(texture, DepthMapUniform, slot);
}

void Material::SetTexture(Ref<Texture> texture, std::string uniform, uint32 slot)
{
    m_textureUniforms.emplace_back(texture, std::move(uniform), slot);
}

void Material::SetShader(const Ref<Shader>& shader)
{
    m_shader = shader;
}

void Material::Use()
{
    m_shader->Bind();

    m_shader->SetUniformVector(AmbientColorUniform, ambient);
    m_shader->SetUniformVector(DiffuseColorUniform, diffuse);
    m_shader->SetUniformVector(SpecularColorUniform, specular);
    m_shader->SetUniformVector(EmissiveColorUniform, emissive);
    m_shader->SetUniformFloat(ShininessUniform, shininess);

    for (auto& [texture, uniformName, textureUnit] : m_textureUniforms)
    {
        m_shader->SetUniformTexture2D(uniformName, texture->GetProxy(), textureUnit);
    }
}

void Material::SetPredefinedTexture(const Ref<Texture>& texture, const std::string& textureUniform, uint32 slot)
{
    const auto uniformName = std::format("{}[{}]", textureUniform, slot);
    SetTexture(texture, uniformName, slot);
}

Ref<Resource> MaterialLoader::Load(const std::string& path, uint32 id)
{
    return nullptr;
}
