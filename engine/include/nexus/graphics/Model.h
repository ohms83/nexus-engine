#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Resource.h"
#include "nexus/core/ResourceLoader.h"
#include "nexus/core/ResourceManager.h"
#include "nexus/graphics/RenderingInterface.h"
#include "Material.h"
#include "Mesh.h"

#include <vector>

struct aiMaterial;
struct aiMesh;
struct aiScene;
struct aiNode;

NXS_NAMESPACE
{
    class Model final : public Resource
    {
    public:
        explicit Model(std::string path, const uint32 resourceId)
            : Resource(std::move(path), resourceId)
        {
        }

        ~Model() override = default;

        void AddMesh(const Ref<Mesh>& mesh);

    private:
        std::vector<Ref<Mesh>> m_meshes;
    };

    class ModelLoader final : public IResourceLoader
    {
    public:
        explicit ModelLoader(
            const Ref<RenderingInterface>& renderingInterface,
            const Ref<TextureManager>& textureManager,
            const Ref<MaterialManager>& materialManager);

        /**
         * @brief Attempts to load a resource from the given path.
         * This method encapsulates the entire loading process for a specific resource type,
         * including file I/O, parsing, and initialization (e.g., GPU upload for textures).
         * @param path The path to the resource file.
         * @param id The unique resource ID.
         * @return A Ref to the loaded IResource on success, or nullptr on failure.
         */
        Ref<Resource> Load(const std::string& path, uint32 id) override;

        /**
         * @brief Returns the std::type_index of the specific IResource type this loader handles.
         * Used by the ResourceManager to map resource types to their respective loaders.
         * @return A std::type_index representing the resource type handled by this loader.
         */
        std::type_index GetResourceType() const override
        {
            return typeid(Model);
        }

    private:
        void ProcessNode(const Ref<Model>& model, const aiNode* node, const aiScene* scene, std::filesystem::path directory);
        void ProcessMesh(const Ref<Model>& model, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path&) const;
        void ProcessMaterial(const Ref<Mesh>& newMesh, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path&) const;
        void ProcessTextures(const Ref<Material>& newMat, const aiMaterial* material, const std::filesystem::path&) const;
        void ProcessTextureType(const aiMaterial* material, int32 type, const std::filesystem::path&,
            const std::function<void(const Ref<Texture>&, uint32)>& setMethod) const;

        Ref<RenderingInterface> m_renderingInterface;
        Ref<TextureManager> m_textureManager;
        Ref<MaterialManager> m_materialManager;
    };

    class ModelManager final : public ResourceManager<Model>
    {
    public:
        ModelManager(
            const Ref<RenderingInterface>& renderingInterface,
            const Ref<TextureManager>& textureManager,
            const Ref<MaterialManager>& materialManager);
    };
}