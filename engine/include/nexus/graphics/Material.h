//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Texture.h"
#include "GraphicsConst.h"
#include "Color.h"
#include "Shader.h"

#include "core/Resource.h"

NXS_NAMESPACE
{
    class Material final : public Resource
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F diffuse{};
        Color3F specular{};
        Color3F emissive{};
        float shininess = 0;

        void SetTexture(Ref<Texture> texture, std::string uniform);
        void SetShader(const Ref<Shader>& shader);

        //! Use this material as for the next drawing operation.
        void Use();

    private:
        Ref<Shader> m_shader;

        struct TextureUniformMap
        {
            Ref<Texture> texture;
            std::string uniformName;
            uint32 textureUnit;
        };
        std::vector<TextureUniformMap> m_textureUniforms;
        DepthFunction depthFunction = DepthFunction::Lesser;
    };
}
