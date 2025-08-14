#pragma once

#include "nexus/NxsDefine.h"
#include "core/Resource.h"
#include "core/ResourceLoader.h"
#include "core/ResourceManager.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"

#include <vector>

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

        void AddMaterial(Ref<Material> material);
        void RemoveMaterial(Ref<Material> material);
        Ref<Material> GetMaterial(uint32 index) const;

        const std::vector<Ref<Material>>& GetMaterialList() const
        {
            return m_materials;
        }

    private:
        Ref<VertexBuffer> m_vertices;
        Ref<IndexBuffer> m_indices;
        std::vector<Ref<Material>> m_materials;
    };

    class ModelLoader : public IResourceLoader
    {
    public:/**
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
    };

    class ModelManager : public ResourceManager<Model>
    {};
}