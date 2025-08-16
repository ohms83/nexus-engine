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
        void RegisterLoader(Ptr<IResourceLoader> loader)
        {
            if (!loader)
            {
                LOG_ERROR(LogResource, "Attempted to register a null loader.");
                return;
            }
            const std::type_index resourceType = loader->GetResourceType();
            if (m_loader)
            {
                LOG_WARNING(LogResource, std::format(
                    "Loader for type '{}' already registered. Overwriting.",
                    resourceType.name()
                ));
            }
            m_loader = std::move(loader);
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
        NODISCARD Ref<ResourceType> Get(const std::string& path)
        {
            std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
            const auto id = m_hasher.Hash32(path);

            // Check if resource is already in the cache
            if (const auto it = m_resourceCache.find(id); it != m_resourceCache.end())
            {
                // Found in cache, try to cast to the requested type
                if (Ref<ResourceType> cached_resource = PTR_CAST<ResourceType>(it->second))
                {
                    return cached_resource;
                }

                // Resource found, but it's of a different type. This indicates a potential path collision
                // or misuse. Handle as an error.
                LOG_ERROR(LogResource, std::format(
                    "Resource '{}' exists in cache but is of type {} (requested {}). Type mismatch.",
                    path, typeid(*it->second).name(), typeid(ResourceType).name()
                ));
                return nullptr; // Or throw std::bad_cast
            }

            // Resource is not in the cache, find the appropriate loader
            const std::type_index resourceType = typeid(ResourceType);
            if (!m_loader)
            {
                LOG_ERROR(LogResource, std::format("No loader registered for type '{}' to load resource '{}'.", resourceType.name(), path));
                return nullptr;
            }

            const Ref<Resource> base_resource = m_loader->Load(path, id);
            if (!base_resource)
            {
                LOG_ERROR(LogResource, std::format("Loader for type '{}' failed to load resource '{}'.", resourceType.name(), path));
                return nullptr;
            }

            // Ensure the loader returned the correct type (it should, by design of GetResourceType)
            Ref<ResourceType> new_resource = PTR_CAST<ResourceType>(base_resource);
            if (!new_resource)
            {
                LOG_ERROR(LogResource, std::format(
                    "Loader for type '{}' returned an unexpected resource type for '{}'. Expected {}, Got {}.",
                    resourceType.name(), path, typeid(ResourceType).name(), typeid(*base_resource).name()
                ));
                return nullptr;
            }

            // Store the newly loaded resource in the cache
            m_resourceCache[id] = new_resource;
            return new_resource;
        }

        NODISCARD Ref<ResourceType> Get(const uint32 id)
        {
            if (const auto itr = m_resourceCache.find(id); itr != m_resourceCache.end())
            {
                return PTR_CAST<ResourceType>(itr->second);
            }
            return nullptr;
        }

        NODISCARD Ref<ResourceType> GetOrCreate(const std::string& path)
        {
            const auto id = m_hasher.Hash32(path);
            if (auto cached_resource = Get(id); cached_resource != nullptr)
            {
                return cached_resource;
            }

            auto new_resource = std::make_shared<ResourceType>(path, id);
            m_resourceCache[id] = new_resource;
            return new_resource;
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
            const auto id = m_hasher.Hash32(path);

            if (const auto it = m_resourceCache.find(id); it != m_resourceCache.end())
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
            uint32 count = 0;
            for (auto itr = m_resourceCache.begin(); itr != m_resourceCache.end();)
            {
                if (auto& resource = itr->second; resource.use_count() <= 1) {
                    itr = m_resourceCache.erase(itr);
                    ++count;
                }
                else {
                    ++itr;
                }
            }
            LOG_INFO(LogResource, std::format("Purged all unused resources. Count={}", count));
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
        //! The central cache mapping resource paths to shared pointers of a base IResource type
        std::unordered_map<uint32, Ref<Resource>> m_resourceCache;
        //! A uniqued_ptr to the concrete loader class.
        Ptr<IResourceLoader> m_loader;
        //! Mutex for thread-safe access to m_resourceCache
        mutable std::mutex m_mutex;
    };
}