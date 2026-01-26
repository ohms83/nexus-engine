//
// Created by nutta on 7/9/2025.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <nexus/NxsDefine.h>
#include <nexus/core/Hasher.h>

#include "Resource.h"
#include "ResourceLoader.h"
#include "../task/OneshotTask.h"

NXS_NAMESPACE
{
    /**
     * @brief An abstract base class for resource managers.
     * 
     */
    class IResourceManager
    {
    public:
        using Iterator = std::unordered_map<uint32, Ref<Resource>>::iterator;
        using CacheResult = std::pair<Iterator, bool>;

        virtual ~IResourceManager() = default;

        virtual void RegisterLoader(Ptr<IResourceLoader> loader) = 0;
        /**
         * @brief Load and cache a resource from the given path.
         * 
         * @param path The path to the resource to load and cache.
         * 
         * @return An iterator to the cached resource, or an end iterator if the resource was not found.
         */
        MAYBE_UNUSED virtual CacheResult Cache(const std::string& path) = 0;
        NODISCARD virtual Ref<Resource> GetResource(const std::string& path) = 0;
        NODISCARD virtual Ref<Resource> GetResource(const uint32 id) const = 0;
        MAYBE_UNUSED virtual Ref<IResourceLoader::LoadResult> GetResourceAsync(const std::string& path, TaskScheduler& scheduler) = 0;
        NODISCARD virtual bool IsExist(const std::string& path) const = 0;
        virtual bool Unload(const std::string& path) = 0;
        virtual void PurgeUnused() = 0;
        /**
         * @brief Remove all cached resources.
         * 
         */
        virtual void EmptyCache() = 0;
    };

    class ResourceManager : public IResourceManager
    {
    public:
        virtual ~ResourceManager() = default;

        /**
         * @brief Registers a resource loader with the manager.
         * Loaders are responsible for knowing how to load specific resource types.
         * Must be called for each resource type before attempting to load it.
         * @param loader A shared_ptr to the IResourceLoader instance.
         */
        void RegisterLoader(Ptr<IResourceLoader> loader) override;

        MAYBE_UNUSED CacheResult Cache(const std::string& path) override;

        /**
         * @brief Create an empty resource.
         * 
         * @tparam T The type of resource to create (must derive from IResource).
         * @param name The unique name (identifier) of the resource.
         * @return A std::shared_ptr to the newly created resource, or nullptr if it already exists.
         */
        template<typename Type>
        requires std::derived_from<Type, Resource>
        NODISCARD Ref<Type> Create(const std::string& name)
        {
            const auto id = m_hasher.Hash32(name);
            if (auto cached_resource = GetResource(id); cached_resource != nullptr)
            {
                return nullptr;
            }

            std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            auto new_resource = std::make_shared<Type>(name, id);
            m_resourceCache[id] = new_resource;
            return new_resource;
        }
        

        /**
         * @brief Retrieves a resource of type T from the manager.
         * If the resource is already cached, it's returned immediately.
         * Otherwise, the appropriate loader is invoked to load it.
         *
         * @tparam T The type of resource to retrieve (must derive from IResource).
         * @param path The unique path (identifier) of the resource.
         * @return A std::shared_ptr to the loaded resource, or nullptr if loading fails or type mismatches.
         */
        template<typename Type>
        requires std::derived_from<Type, Resource>
        NODISCARD Ref<Type> Get(const std::string& path)
        {
            return PTR_CAST<Type>(GetResource(path));
        }

        /**
         * @brief Retrieves a cached resource or loads it if not already cached.
         * 
         * If the resource is already cached, it's returned immediately.
         * Otherwise, the registered loader is invoked to load and cache it.
         *
         * @param path The unique path (identifier) of the resource.
         * @return A shared_ptr to the resource of type Type, or nullptr if loading fails.
         */
        NODISCARD Ref<Resource> GetResource(const std::string& path) override;

        /**
         * @brief Retrieves a cached resource by its ID.
         * 
         * @param id The unique ID of the resource.
         * @return A shared_ptr to the resource, or nullptr if not found.
         */
        NODISCARD Ref<Resource> GetResource(const uint32 id) const override;

        NODISCARD bool IsExist(const std::string& path) const override
        {
            const auto id = m_hasher.Hash32(path);
            return GetResource(id) != nullptr;
        }

        /**
         * @brief Requests a resource to be loaded asynchronously.
         *
         * This function initiates an asynchronous loading process for a resource from the specified path.
         * It first checks for a cached version of the resource. If it's already in the cache, it
         * returns a ready-to-use resource immediately. If not, it checks if the resource is
         * currently loading. If so, it returns the existing loading task's @c LoadResult.
         * Otherwise, it starts a new asynchronous loading operation via a registered resource loader
         * and schedules a periodic task to monitor its status.
         *
         * @tparam Type The type of the resource to be loaded.
         * @param path The file path to the resource.
         * @param scheduler A reference to the TaskScheduler to manage the asynchronous task.
         * @return A Ref to an @c IResourceLoader::LoadResult object that can be used to monitor the
         * loading status of the requested resource. Returns @c nullptr if no loader is
         * registered for the specified resource type.
         * @note This function is thread-safe as it uses a mutex to protect access to the list of
         * currently loading resources.
         */
        MAYBE_UNUSED Ref<IResourceLoader::LoadResult> GetResourceAsync(const std::string& path, TaskScheduler& scheduler) override;

        /**
         * @brief Unloads a specific resource from the cache.
         * The resource's destructor will be called when its shared_ptr reference count drops to zero.
         * @param path The path of the resource to unload.
         * @return True if the resource was found and removed from cache, false otherwise.
         */
        bool Unload(const std::string& path) override;

        //! Release all the resources that no one else but the manager is holding (ref count = 1).
        void PurgeUnused() override;

        /**
         * @brief Clears all resources from the cache.
         * All resources will be destroyed when their shared_ptr ref counts drop to zero.
         */
        void EmptyCache() override
        {
            // Optional: std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            m_resourceCache.clear();
        }

    protected:
        Hasher m_hasher;
        //! The central cache mapping resource paths to shared pointers of a base IResource type
        std::unordered_map<uint32, Ref<Resource>> m_resourceCache;
        //! A list of resources that are currently loading.
        std::vector<Ref<IResourceLoader::LoadResult>> m_loadingResources;
        //! A uniqued_ptr to the concrete loader class.
        Ptr<IResourceLoader> m_loader;
        //! Mutex for thread-safe access to m_resourceCache
        mutable std::mutex m_mutex;
    };
}