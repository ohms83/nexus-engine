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
#include "nexus/core/ResourceLoader.h"
#include "nexus/core/ResourceManager.h"

NXS_NAMESPACE
{
    class Material final : public Resource
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F ambient{1, 1, 1};;
        Color3F diffuse{1, 1, 1};
        Color3F specular{1, 1, 1};
        Color3F emissive{};
        float shininess = 0;
        BlendMode blendMode = BlendMode::None;
        bool wireframe = false;
        bool cull = false;
        bool depthTest = false;
        bool depthWrite = false;

        void SetDiffuseTexture(const Ref<Texture>& texture, uint32 slot);
        void SetSpecularTexture(const Ref<Texture>& texture, uint32 slot);
        void SetEmissiveTexture(const Ref<Texture>& texture, uint32 slot);
        void SetNormalTexture(const Ref<Texture>& texture, uint32 slot);
        void SetHeightTexture(const Ref<Texture>& texture, uint32 slot);
        void SetTexture(Ref<Texture> texture, std::string uniform, uint32 slot);
        void SetShader(const Ref<Shader>& shader);

        //! Use this material as for the next drawing operation.
        void Use();

        static const std::string AmbientColorUniform;
        static const std::string DiffuseColorUniform;
        static const std::string SpecularColorUniform;
        static const std::string EmissiveColorUniform;
        static const std::string ShininessUniform;
        static const std::string BlendModeUniform;
        static const std::string DiffuseMapUniform;
        static const std::string SpecularMapUniform;
        static const std::string EmissiveMapUniform;
        static const std::string NormalMapUniform;
        static const std::string DepthMapUniform;

    private:
        void SetPredefinedTexture(const Ref<Texture>& texture, const std::string& textureUniform, uint32 slot);
        Ref<Shader> m_shader;

        struct TextureUniformMap
        {
            Ref<Texture> texture;
            std::string uniformName;
            uint32 slot;
        };
        std::vector<TextureUniformMap> m_textureUniforms;
        DepthFunction depthFunction = DepthFunction::Lesser;
    };

    class MaterialLoader final : public IResourceLoader
    {
    public:
        Ref<Resource> Load(const std::string& path, uint32 id) override;

        std::type_index GetResourceType() const override
        {
            return typeid(Material);
        }
    };

    class MaterialManager final : public ResourceManager<Material>
    {
    public:
        MaterialManager()
        {
            RegisterLoader(std::make_unique<MaterialLoader>());
        }
    };
}
