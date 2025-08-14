//
// Created by nutta on 7/12/2025.
//

#include "graphics/Material.h"

USING_NAMESPACE_NXS;

Material::Material(std::string path, const uint32 resourceId)
    : Resource(std::move(path), resourceId)
{
}

void Material::SetTexture(Ref<Texture> texture, std::string uniform)
{
    m_textureUniforms.emplace_back(texture, std::move(uniform));
}

void Material::SetShader(const Ref<Shader>& shader)
{
    m_shader = shader;
}

void Material::Use()
{
    m_shader->Bind();

    m_shader->SetUniformVector("uMatDiffuse", diffuse);
    m_shader->SetUniformVector("uMatSpecular", specular);
    m_shader->SetUniformVector("uMatEmissive", emissive);
    m_shader->SetUniformFloat("uMatShininess", shininess);

    for (auto& [texture, uniformName, textureUnit] : m_textureUniforms)
    {
        m_shader->SetUniformTexture2D(uniformName, texture->GetProxy(), textureUnit);
    }
}
