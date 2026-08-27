#include "core/resource/ResourceManager.h"

USING_NAMESPACE_NXS;

void ResourceManager::RegisterLoader(std::type_index type, Ptr<IResourceLoader> loader)
{
    if (!loader)
    {
        LOG_ERROR(LogResource, "Attempted to register a null loader.");
        return;
    }
    if (m_loaders.find(type) != m_loaders.end())
    {
        LOG_WARNING(LogResource, std::format("Loader for type '{}' already registered. Overwriting.", type.name()));
    }
    m_loaders[type] = std::move(loader);
}

IResourceLoader* ResourceManager::GetLoader(std::type_index type)
{
    if (auto loader = m_loaders.find(type); loader != m_loaders.end())
    {
        return loader->second.get();
    }
    return nullptr;
}

ResourceManager::ResourceMap& ResourceManager::GetResourceMap(std::type_index type)
{
    if (auto itr = m_cacheMaps.find(type); itr != m_cacheMaps.end())
    {
        return itr->second;
    }
    return m_cacheMaps.emplace(type, ResourceMap{}).first->second;
}

ResourceManager::CacheResult ResourceManager::Cache(std::type_index type, const std::string& path)
{
    const auto id = m_hasher.Hash32(path);
    auto& resourceMap = GetResourceMap(type);
    if (auto itr = resourceMap.find(id); itr != resourceMap.end())
    {
        // Resource is already cached.
        return { itr, true };
    }

    std::lock_guard<std::mutex> lock(m_mutexMap[type]); // For thread-safety

    auto loader = GetLoader(type);
    if (loader == nullptr)
    {
        LOG_ERROR(LogResource, std::format("No loader registered to load resource type '{}'.", type.name()));
        return { resourceMap.end(), false };
    }

    const auto new_resource = loader->Load(path, id);
    if (!new_resource)
    {
        LOG_ERROR(LogResource, std::format("Failed to load resource '{}'.", path));
        return { resourceMap.end(), false };
    }
    // Store the newly loaded resource in the cache
    return { resourceMap.insert({ id, new_resource }).first, true };
}

Ref<IResourceLoader::LoadResult> ResourceManager::GetResourceAsync(std::type_index type, const std::string& path, TaskScheduler& scheduler)
{
    const auto id = m_hasher.Hash32(path);
    auto& resourceCache = GetResourceMap(std::type_index(type));
    if (auto cached_resource = resourceCache.find(id); cached_resource != resourceCache.end())
    {
        const auto result = std::make_shared<IResourceLoader::LoadResult>();
        result->path = path;
        result->resource = cached_resource->second;
        result->status = IResourceLoader::LoadResult::Status::Ready;
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutexMap[type]); // For thread-safety
    if (auto loadResult = std::ranges::find_if(m_loadingResources, [path](Ref<IResourceLoader::LoadResult> loading_resource)
    {
        return loading_resource->path == path;
    }); loadResult != m_loadingResources.end())
    {
        // The requested resource is currently loading.
        return *loadResult;
    }

    auto loader = GetLoader(type);
    if (loader == nullptr)
    {
        LOG_ERROR(LogResource, std::format("No loader registered to load resource type '{}'.", type.name()));
        return nullptr;
    }

    auto result = loader->LoadAsync(path, id, scheduler, [&resourceCache = GetResourceMap(type), id, path](Ref<Resource> new_resource) {
        if (!new_resource)
        {
            LOG_ERROR(LogResource, std::format("Failed to load resource '{}'.", path));
            return;
        }
        resourceCache[id] = new_resource;
        // LOG_DEBUG(LogResource, std::format("Loaded resource success: '{}'.", path));
    });

    if (result->status == IResourceLoader::LoadResult::Status::Loading) {
        m_loadingResources.push_back(result);
    }

    // Periodically checking for resource loading status.
    scheduler.ScheduleTask(std::make_shared<OneshotTask>([result, &mutex = m_mutexMap[type], &loadingResources = m_loadingResources]()
    {
        if (result->status == IResourceLoader::LoadResult::Status::Ready)
        {
            loadingResources.erase(std::ranges::find(loadingResources, result));
            return false;
        }
        return true;
    }));
    return result;
}

bool ResourceManager::Unload(std::type_index type, const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_mutexMap[type]); // For thread-safety
    const auto id = m_hasher.Hash32(path);

    auto& resourceMap = GetResourceMap(type);
    if (const auto it = resourceMap.find(id); it != resourceMap.end())
    {
        resourceMap.erase(it);
        return true;
    }
    LOG_WARNING(LogResource, std::format("Attempted to unload unknown resource '{}'.", path));
    return false;
}

void ResourceManager::PurgeUnused(std::type_index type)
{
    uint32 count = 0;
    auto& resourceMap = GetResourceMap(type);
    for (auto itr = resourceMap.begin(); itr != resourceMap.end();)
    {
        if (auto& resource = itr->second; resource.use_count() <= 1) {
            itr = resourceMap.erase(itr);
            ++count;
        }
        else {
            ++itr;
        }
    }
    LOG_INFO(LogResource, std::format("Purged unused resources of type '{}'. Count={}", type.name(), count));
}