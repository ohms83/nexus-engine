//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Resource.h"
#include "Texture.h"
#include "nexus/graphics/Color.h"
#include "nexus/graphics/Shader.h"

NXS_NAMESPACE
{
    class Material final : public Resource
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F diffuse;
        Color3F specular;
        Color3F emissive;
        float shininess = 0;

        void SetDiffuseMap(Ref<Texture> texture);
        void SetNormalMap(Ref<Texture> texture);
        void SetSpecularMap(Ref<Texture> texture);

        void SetShader(Ref<Shader> shader);

        //! Use this material as for the next drawing operation.
        void Use();

    private:
        struct TextureUniformMap
        {
            Ref<Texture> texture;
            std::string uniformName;
        };

        Ref<Texture> m_diffuseMap;
        Ref<Texture> m_specularMap;
        Ref<Texture> m_normalMap;
        Ref<Shader> m_shader;

        std::vector<TextureUniformMap> m_textureUniforms;
    };
}
