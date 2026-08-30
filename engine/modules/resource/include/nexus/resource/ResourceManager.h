//
// Created by nutta on 7/9/2025.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <concepts>

#include "nexus/NxsDefine.h"
#include "nexus/core/Hasher.h"

#include "Resource.h"
#include "ResourceLoader.h"
#include "nexus/task/OneshotTask.h"

NXS_NAMESPACE
{
    class ResourceManager
    {
    public:
        /**< A mapping of resource paths to shared pointers of a base IResource type */
        using ResourceMap = std::unordered_map<uint32_t, Ref<Resource>>;
        using ResourceMapIterator = ResourceMap::iterator;
        using ResourceMapConstIterator = ResourceMap::const_iterator;
        using ResourceMapList = std::map<std::type_index, ResourceMap>;
        using CacheResult = std::pair<ResourceMapIterator, bool>;
        /**
         * @brief A mapping of resource type to their respective loaders.
         */
        using LoaderMap = std::unordered_map<std::type_index, Ptr<IResourceLoader>>;

        virtual ~ResourceManager() = default;
        /**
         * @brief Registers a resource loader for a specific resource type.
         * 
         * Loaders are responsible for knowing how to load specific resource types.
         * Must be called for each resource type before attempting to load it.
         * 
         * @param type The std::type_index of the resource type.
         * @param loader A unique_ptr to the IResourceLoader instance.
         */
        void RegisterLoader(std::type_index type, Ptr<IResourceLoader> loader);

        MAYBE_UNUSED CacheResult Cache(std::type_index type, const std::string& path);

        IResourceLoader* GetLoader(std::type_index type);

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
            if (auto cached_resource = Get<Type>(name); cached_resource != nullptr)
            {
                return cached_resource;
            }

            auto& mutex = m_mutexMap[typeid(Type)];
            std::lock_guard<std::mutex> lock(mutex); // For thread-safety
            const auto id = m_hasher.Hash32(name);
            auto& cacheMap = GetResourceMap(typeid(Type));
            auto new_resource = std::make_shared<Type>(name, id);
            cacheMap[id] = new_resource;
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
            const auto cached = Cache(typeid(Type), path);
            return cached.second ? PTR_CAST<Type>(cached.first->second) : nullptr;
        }

        Ref<Resource> GetResource(std::type_index type, const std::string& path)
        {
            const auto cached = Cache(type, path);
            return cached.first->second;
        }

        ResourceMapConstIterator Find(std::type_index type, const std::string& path) const
        {
            const auto id = m_hasher.Hash32(path);
            if (auto itr = m_cacheMaps.find(type); itr != m_cacheMaps.end())
            {
                return itr->second.find(m_hasher.Hash32(path));
            }
            return ResourceMapConstIterator{};
        }

        NODISCARD bool IsExist(std::type_index type, const std::string& path) const
        {
            if (auto itr = m_cacheMaps.find(type); itr != m_cacheMaps.end())
            {
                return itr->second.contains(m_hasher.Hash32(path));
            }
            return false;
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
        MAYBE_UNUSED Ref<IResourceLoader::LoadResult> GetResourceAsync(std::type_index type, const std::string& path, TaskScheduler& scheduler);

        /**
         * @brief Unloads a specific resource from the cache.
         * The resource's destructor will be called when its shared_ptr reference count drops to zero.
         * @param path The path of the resource to unload.
         * @return True if the resource was found and removed from cache, false otherwise.
         */
        bool Unload(std::type_index type, const std::string& path);

        //! Release all the resources that no one else but the manager is holding (ref count = 1).
        void PurgeUnused(std::type_index type);

        /**
         * @brief Clears all resources from the cache.
         * All resources will be destroyed when their shared_ptr ref counts drop to zero.
         */
        void EmptyCache()
        {
            for (auto& [type, cacheMap] : m_cacheMaps)
            {
                std::lock_guard<std::mutex> lock(m_mutexMap[type]); // For thread-safety
                cacheMap.clear();
            }
            m_cacheMaps.clear();
        }

    protected:
        ResourceMap& GetResourceMap(std::type_index type);

        Hasher m_hasher;
        ResourceMapList m_cacheMaps;
        LoaderMap m_loaders;
        //! A list of resources that are currently loading.
        std::vector<Ref<IResourceLoader::LoadResult>> m_loadingResources;
        //! Mutex for thread-safe access to the resource mapes
        mutable std::map<std::type_index, std::mutex> m_mutexMap;
    };
}