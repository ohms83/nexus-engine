//
// Created by nutta on 8/18/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/resource/ResourceLoader.h"

#include "RenderingInterface.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "ShaderManager.h"

#include <filesystem>

NXS_NAMESPACE
{
    class ModelLoader final : public IResourceLoader
    {
    public:
        explicit ModelLoader(
            const Ref<RenderingInterface>& renderingInterface,
            const Ref<TextureManager>& textureManager,
            const Ref<MaterialManager>& materialManager,
            const Ref<ShaderManager>& shaderManager);

        /**
         * @brief Attempts to load a resource from the given path.
         * This method encapsulates the entire loading process for a specific resource type,
         * including file I/O, parsing, and initialization (e.g., GPU upload for textures).
         * @param path The path to the resource file.
         * @param id The unique resource ID.
         * @return A Ref to the loaded IResource on success, or nullptr on failure.
         */
        MAYBE_UNUSED Ref<Resource> Load(const std::string& path, uint32 id) override;

        MAYBE_UNUSED Ref<LoadResult> LoadAsync(
            const std::string& path,
            uint32 id,
            TaskScheduler& scheduler,
            Callback onFinishCallback) override;

    private:
        void ProcessNode(const Ref<Model>& model, const aiNode* node, const aiScene* scene, std::filesystem::path directory);
        void ProcessMesh(const Ref<Model>& model, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path&) const;
        void ProcessMaterial(const Ref<Mesh>& newMesh, const aiMesh* mesh, const aiScene* scene, const std::filesystem::path&) const;
        void ProcessTextures(const Ref<Material>& newMat, const aiMaterial* material, const std::filesystem::path&) const;
        void ComputeBoundingVolume(const Ref<Model>& model);

        Ref<RenderingInterface> m_renderingInterface;
        Ref<TextureManager> m_textureManager;
        Ref<MaterialManager> m_materialManager;
        Ref<ShaderManager> m_shaderManager;
    };
}
