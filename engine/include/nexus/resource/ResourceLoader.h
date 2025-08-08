#pragma once

#include <string>
#include <typeindex> // For std::type_index (used in ResourceManager for mapping loaders)

#include "Resource.h" // Needed for IResource in virtual function

NXS_NAMESPACE
{
    /**
     * @brief Base interface for all resource loaders.
     * Each concrete loader (e.g., TextureLoader, ModelLoader) implements this to
     * define how a specific type of resource is loaded from a path.
     */
    class IResourceLoader
    {
    public:
        virtual ~IResourceLoader() = default;

        /**
         * @brief Attempts to load a resource from the given path.
         * This method encapsulates the entire loading process for a specific resource type,
         * including file I/O, parsing, and initialization (e.g., GPU upload for textures).
         * @param path The path to the resource file.
         * @param id The unique resource ID.
         * @return A Ref to the loaded IResource on success, or nullptr on failure.
         */
        virtual Ref<Resource> Load(const std::string& path, uint32 id) = 0;

        /**
         * @brief Returns the std::type_index of the specific IResource type this loader handles.
         * Used by the ResourceManager to map resource types to their respective loaders.
         * @return A std::type_index representing the resource type handled by this loader.
         */
        virtual std::type_index GetResourceType() const = 0;
    };

} // NXS_NAMESPACE