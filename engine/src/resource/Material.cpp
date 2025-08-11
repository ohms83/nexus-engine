//
// Created by nutta on 7/12/2025.
//

#include <nexus/resource/Material.h>

USING_NAMESPACE_NXS;

Material::Material(std::string path, const uint32 resourceId)
    : Resource(path, resourceId)
{
}

void Material::SetDiffuseMap(Ref<Texture> texture)
{
    m_diffuseMap = texture;
}

void Material::SetNormalMap(Ref<Texture> texture)
{
    m_normalMap = texture;
}

void Material::SetSpecularMap(Ref<Texture> texture)
{
    m_specularMap = texture;
}

void Material::SetShader(Ref<Shader> shader)
{
    m_shader = shader;
}