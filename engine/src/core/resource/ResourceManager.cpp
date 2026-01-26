#include "core/resource/ResourceManager.h"

USING_NAMESPACE_NXS;

void ResourceManager::RegisterLoader(Ptr<IResourceLoader> loader)
{
    if (!loader)
    {
        LOG_ERROR(LogResource, "Attempted to register a null loader.");
        return;
    }
    if (m_loader)
    {
        LOG_WARNING(LogResource, std::format("Loader for type already registered. Overwriting."));
    }
    m_loader = std::move(loader);
}

IResourceManager::CacheResult ResourceManager::Cache(const std::string& path)
{
    const auto id = m_hasher.Hash32(path);
    if (auto itr = m_resourceCache.find(id); itr != m_resourceCache.end())
    {
        // Resource is already cached.
        return { itr, true };
    }

    std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety

    if (!m_loader)
    {
        LOG_ERROR(LogResource, std::format("No loader registered to load resource '{}'.", path));
        return { m_resourceCache.end(), false };
    }

    const auto new_resource = m_loader->Load(path, id);
    if (!new_resource)
    {
        LOG_ERROR(LogResource, std::format("Failed to load resource '{}'.", path));
        return { m_resourceCache.end(), false };
    }
    // Store the newly loaded resource in the cache
    return { m_resourceCache.insert({ id, new_resource }).first, true };
}

Ref<Resource> ResourceManager::GetResource(const std::string& path)
{
    const auto cached = Cache(path);
    return cached.first->second;
}

Ref<Resource> ResourceManager::GetResource(const uint32 id) const
{
    std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
    if (const auto itr = m_resourceCache.find(id); itr != m_resourceCache.end())
    {
        return PTR_CAST<Resource>(itr->second);
    }
    return nullptr;
}

Ref<IResourceLoader::LoadResult> ResourceManager::GetResourceAsync(const std::string& path, TaskScheduler& scheduler)
{
    const auto id = m_hasher.Hash32(path);
    if (auto cached_resource = GetResource(id))
    {
        const auto result = std::make_shared<IResourceLoader::LoadResult>();
        result->path = path;
        result->resource = cached_resource;
        result->status = IResourceLoader::LoadResult::Status::Ready;
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutex); // For thread-safety
    if (auto loadResult = std::ranges::find_if(m_loadingResources, [path](Ref<IResourceLoader::LoadResult> loading_resource)
    {
        return loading_resource->path == path;
    }); loadResult != m_loadingResources.end())
    {
        // The requested resource is currently loading.
        return *loadResult;
    }

    if (!m_loader)
    {
        LOG_ERROR(LogResource, std::format("No loader registered to load resource '{}'.", path));
        return nullptr;
    }

    auto result = m_loader->LoadAsync(path, id, scheduler, [&resourceCache = m_resourceCache, id, path](Ref<Resource> new_resource) {
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
    scheduler.ScheduleTask(std::make_shared<OneshotTask>([result, &mutex = m_mutex, &loadingResources = m_loadingResources]()
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

bool ResourceManager::Unload(const std::string& path)
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

void ResourceManager::PurgeUnused()
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