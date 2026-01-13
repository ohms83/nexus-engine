//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Texture.h"
#include "GraphicsConst.h"
#include "Color.h"
#include "Shader.h"
#include "RenderingInterface.h"
#include "nexus/io/Serializable.h"

#include "nexus/core/Resource.h"

NXS_NAMESPACE
{
    class TextureManager;
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

    class Material final : public Resource, public ISerializable
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F ambient {0.2f, 0.2f, 0.2f};
        Color3F diffuse {0.8f, 0.8f, 0.8f};
        Color3F specular {Color3F::Black};
        Color3F emissive {Color3F::Black};
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

        void SetShader(Ref<Shader> shader);
        Ref<Shader> GetShader() const
        {
            return m_shader;
        }

        //! Create a default shader based on the material properties.
        void CreateDefaultShader(Ref<RenderingInterface> renderingInterface);

        size_t TextureCount() const { return m_textures.size(); }

        // Serialization
        VariantData Serialize() const override;
        void Deserialize(const VariantData& data) override;

        void Use();

        // Resolve referenced textures and optionally shaders by using resource managers.
        void Resolve(class TextureManager& textureManager, RenderingInterface* renderingInterface = nullptr);

        // Accessors for texture metadata
        NODISCARD std::string GetTexturePath(uint32 slot) const;
        NODISCARD std::string GetTextureUniform(uint32 slot) const;
        NODISCARD TextureType GetTextureType(uint32 slot) const;

    private:
        // TODO: Deprecate this and move all the logic to ShaderGenerator.
        bool DetermineShaderPaths(std::string& vertexShader, std::string& fragmentShader);

        Ref<Shader> m_shader;
        std::string m_shaderPath;

        struct TextureInfo
        {
            Ref<Texture> texture;
            TextureType type;
            std::string uniformName;
            std::string path;
        };
        std::vector<TextureInfo> m_textures;
    };
}
