//
// Created by nutta on 8/18/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/ResourceLoader.h"
#include "nexus/graphics/RenderingInterface.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "MaterialManager.h"

NXS_NAMESPACE
{
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

        Ref<RenderingInterface> m_renderingInterface;
        Ref<TextureManager> m_textureManager;
        Ref<MaterialManager> m_materialManager;
    };
}
