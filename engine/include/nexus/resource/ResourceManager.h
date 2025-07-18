//
// Created by nutta on 7/9/2025.
//
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <nexus/NxsDefine.h>
#include <nexus/core/Hasher.h>

NXS_NAMESPACE
{
    class Resource;

    template<typename ResourceType>
    requires std::derived_from<ResourceType, Resource>
    class ResourceManager
    {
    public:
        /**
         * Get a resource from the specified path. If it hasn't been loaded yet,
         * it will be loaded.
         * @param filepath Resource's filepath.
         * @return A pointer of @c ResourceType or null, if the resource is not found.
         */
        NODISCARD virtual Ref<ResourceType> Get(const std::string& filepath)
        {
            const auto hash = m_hasher.Hash32(filepath);

            if (const auto itr = m_resources.find(hash); itr != m_resources.end()) {
                return itr->second;
            }

            auto resource = std::make_shared<ResourceType>(hash);
            if (!resource->Load(filepath)) {
                return nullptr;
            }

            m_resources[hash] = resource;
            return resource;
        }

        NODISCARD virtual Ref<ResourceType> Get(uint32 hash) const
        {
            if (const auto itr = m_resources.find(hash); itr != m_resources.end()) {
                return itr->second;
            }
            return nullptr;
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

        static ResourceManager& GetInstance()
        {
            static ResourceManager instance;
            return instance;
        }

    protected:
        Hasher m_hasher;
        std::unordered_map<uint32, Ref<ResourceType>> m_resources;
    };
}