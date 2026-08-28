#pragma once

#include "Resource.h"
#include "nexus/memory/Buffer.h"
#include <string>
#include <atomic>

#include "../task/TaskScheduler.h"

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

        struct LoadResult
        {
            enum class Status
            {
                Invalid,
                Ready,
                Failed,
                Loading,
            };

            //! Resource path
            std::string path;
            std::string error;
            std::atomic<Status> status{Status::Invalid};
            Ref<Resource> resource;
        };

        virtual ~IResourceLoader() = default;

        /**
         * @brief Attempts to load a resource from the given path.
         * This method encapsulates the entire loading process for a specific resource type,
         * including file I/O, parsing, and initialization (e.g., GPU upload for textures).
         * @param path The path to the resource file.
         * @param id The unique resource ID.
         * @return A Ref to the loaded IResource on success, or nullptr on failure.
         */
        MAYBE_UNUSED virtual Ref<Resource> Load(const std::string& path, uint32_t id) = 0;
        /**
         * @brief Attempts to load a resource asynchronously from a given path.
         *
         * This function initiates the asynchronous loading of a resource. The actual
         * loading (e.g., file I/O, parsing, GPU upload) is delegated to a worker
         * thread managed by the provided @p scheduler. This method returns a
         * `LoadResult` immediately, which can be used to monitor the loading status
         * without blocking the calling thread. The `onFinishCallback` is invoked
         * on a worker thread once the loading is complete.
         *
         * @param path The file path to the resource.
         * @param id The unique identifier for the resource.
         * @param scheduler A reference to the TaskScheduler to manage the asynchronous task.
         * @param onFinishCallback A callback function that is invoked when the resource has finished loading.
         * @return A Ref to an `IResourceLoader::LoadResult` object that can be used to
         * monitor the loading status.
         */
        MAYBE_UNUSED virtual Ref<LoadResult> LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback) = 0;
    };

} // NXS_NAMESPACE