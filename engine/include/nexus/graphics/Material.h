//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Texture.h"
#include "GraphicsConst.h"
#include "Color.h"
#include "Shader.h"

#include "nexus/core/Resource.h"

NXS_NAMESPACE
{
    //! A list of enumerations representing commonly used texture types.
    enum class TextureType
    {
        //! @brief Undefined texture type.
        Undefined,
        //! @brief Diffuse color map (albedo).
        Diffuse,
        //! @brief Normal map, used for surface detail.
        Normal,
        //! @brief Depth map (height map).
        Depth,
        //! @brief Specular highlight map.
        Specular,
        //! @brief Emissive map, for self-illuminating surfaces.
        Emissive,
        /**
         * @brief Ambient Occlusion (AO) map.
         * While conventionally named "ambient map" (`map_Ka` in OBJ/MTL), its purpose
         * in modern pipelines is to store pre-calculated ambient light occlusion.
         */
        Ambient,
        //! @brief Opacity map, used for transparency/translucency.
        Opacity,
        //! @brief Metallic map, used in PBR workflows.
        Metalness,
        //! @brief Roughness map, used in PBR workflows.
        Roughness,
        //! @brief Light map, for pre-baked global illumination.
        LightMap,
    };

    class Material final : public Resource
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F ambient {};
        Color3F diffuse {};
        Color3F specular {};
        Color3F emissive {};
        float shininess = 0;
        bool wireframe = false;
        bool cull = false;
        bool depthTest = false;
        bool depthWrite = false;
        BlendMode blendMode = BlendMode::None;
        DepthFunction depthFunction = DepthFunction::Lesser;

        /**
         * Add the given texture to the texture list.
         * @param texture Texture to add.
         * @param type Texture type.
         * @return Newly added texture's slot, or -1 if the error occured.
         */
        MAYBE_UNUSED int32 AddTexture(Ref<Texture> texture, TextureType type);
        /**
         * Add the given texture to the texture list.
         * @param texture Texture to add.
         * @param uniform A uniform name of this texture.
         * @return Newly added texture's slot.
         */
        MAYBE_UNUSED int32 AddTexture(Ref<Texture> texture, std::string uniform);
        Ref<Texture> GetTexture(uint32 slot);

        //! Check whether a texture of the specified type is used in this material.
        bool HasTextureType(TextureType type) const;

        void SetShader(const Ref<Shader>& shader);
        Ref<Shader> GetShader() const
        {
            return m_shader;
        }

        //! Create a default shader based on the material properties.
        void CreateDefaultShader(const Ref<RenderingInterface>& renderingInterface);

        size_t TextureCount() const { return m_textures.size(); }

        void Use();

    private:
        void DetermineShaderPaths(std::string& vertexShader, std::string& fragmentShader);

        Ref<Shader> m_shader;

        struct TextureInfo
        {
            Ref<Texture> texture;
            TextureType type;
            std::string uniformName;
        };
        std::vector<TextureInfo> m_textures;
    };
}
