#pragma once

#include "Resource.h"
#include "nexus/memory/Buffer.h"
#include <string>

#include "task/TaskScheduler.h"

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
        using Callback = std::function<void(Ref<Resource>)>;
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

        virtual void LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback) = 0;
    };

} // NXS_NAMESPACE