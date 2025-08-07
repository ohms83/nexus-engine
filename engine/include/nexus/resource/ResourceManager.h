//
// Created by nutta on 7/9/2025.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <nexus/NxsDefine.h>
#include <nexus/core/Hasher.h>

#include "Resource.h"

NXS_NAMESPACE
{
    template<typename ResourceType>
    requires std::derived_from<ResourceType, Resource>
    class ResourceManager
    {
    public:
        virtual ~ResourceManager() = default;

        /**
         * @brief Registers a resource loader with the manager.
         * Loaders are responsible for knowing how to load specific resource types.
         * Must be called for each resource type before attempting to load it.
         * @param loader A shared_ptr to the IResourceLoader instance.
         */
        void RegisterLoader(Ref<IResourceLoader> loader)
        {
            if (!loader)
            {
                LOG_ERROR(LogResource, "Attempted to register a null loader.");
                return;
            }
            std::type_index resourceType = loader->GetResourceType();
            if (m_loaders.count(resourceType))
            {
                LOG_WARNING(LogResource, std::format(
                    "Loader for type '{}' already registered. Overwriting.",
                    resourceType.name()
                ));
            }
            m_loaders[resourceType] = std::move(loader);
            LOG_INFO(LogResource, std::format("Registered loader for type '{}'.", resourceType.name()));
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
        template<typename T>
        std::shared_ptr<T> Get(const std::string& path)
        {
            std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            const auto hash = m_hasher.Hash32(path);

            // Check if resource is already in cache
            if (auto it = m_resourceCache.find(hash); it != m_resourceCache.end())
            {
                // Found in cache, try to cast to the requested type
                Ref<T> cached_resource = PTR_CAST<T>(it->second);
                if (cached_resource)
                {
                    return cached_resource;
                }
                else
                {
                    // Resource found, but it's of a different type. This indicates a potential path collision
                    // or misuse. Handle as an error.
                    LOG_ERROR(LogResource, std::format(
                        "Resource '{}' exists in cache but is of type {} (requested {}). Type mismatch.",
                        path, typeid(*it->second).name(), typeid(T).name()
                    ));
                    return nullptr; // Or throw std::bad_cast
                }
            }

            // Resource not in cache, find the appropriate loader
            std::type_index resourceType = typeid(T);
            auto loader_it = m_loaders.find(resourceType);
            if (loader_it == m_loaders.end())
            {
                LOG_ERROR(LogResource, std::format("No loader registered for type '{}' to load resource '{}'.", resourceType.name(), path));
                return nullptr;
            }

            Ref<IResource> base_resource = loader_it->second->Load(path);
            if (!base_resource)
            {
                LOG_ERROR(LogResource, std::format("Loader for type '{}' failed to load resource '{}'.", resourceType.name(), path));
                return nullptr;
            }

            // Ensure the loader returned the correct type (it should, by design of GetResourceType)
            Ref<T> new_resource = PTR_CAST<T>(base_resource);
            if (!new_resource)
            {
                NXS_LOG_ERROR(LogResource, std::format(
                    "Loader for type '{}' returned an unexpected resource type for '{}'. Expected {}, Got {}.",
                    resourceType.name(), path, typeid(T).name(), typeid(*base_resource).name()
                ));
                return nullptr;
            }

            // Store the newly loaded resource in the cache
            m_resourceCache[path] = new_resource;
            return new_resource;
        }

        NODISCARD virtual Ref<ResourceType> Get(uint32 hash) const
        {
            if (const auto itr = m_resources.find(hash); itr != m_resources.end()) {
                return itr->second;
            }
            return nullptr;
        }

        /**
         * @brief Unloads a specific resource from the cache.
         * The resource's destructor will be called when its shared_ptr reference count drops to zero.
         * @param path The path of the resource to unload.
         * @return True if the resource was found and removed from cache, false otherwise.
         */
        bool Unload(const std::string& path)
        {
            std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            const auto hash = m_hasher.Hash32(path);

            auto it = m_resourceCache.find(hash);
            if (it != m_resourceCache.end())
            {
                m_resourceCache.erase(it);
                return true;
            }
            LOG_WARNING(LogResource, std::format("Attempted to unload unknown resource '{}'.", path));
            return false;
        }

        //! Release all the resources that no one else but the manager is holding (ref count = 1).
        void PurgeUnused()
        {
            for (auto itr = m_resources.begin(); itr != m_resources.end();)
            {
                if (auto& resource = itr->second; resource.use_count() <= 1) {
                    itr = m_resources.erase(itr);
                }
                else {
                    ++itr;
                }
            }
        }
        /**
         * @brief Clears all resources from the cache.
         * All resources will be destroyed when their shared_ptr ref counts drop to zero.
         */
        void ClearAllResources()
        {
            // Optional: std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            LOG_INFO(LogResource, "Clearing all resources from cache.");
            m_resourceCache.clear();
        }

    protected:
        Hasher m_hasher;
        //! The central cache mapping resource paths to shared pointers of base IResource type
        std::unordered_map<uint32, std::shared_ptr<IResource>> m_resourceCache;
        //! Registry of resource loaders, keyed by the type_index of the resource they load
        std::unordered_map<std::type_index, std::shared_ptr<IResourceLoader>> m_loaders;
        //! Mutex for thread-safe access to m_resourceCache and m_loaders
        mutable std::mutex m_mutex;
    };
}