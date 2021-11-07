#ifndef __NXS_RESOURCE_MANAGER_H__
#define __NXS_RESOURCE_MANAGER_H__

#include "NxsDefine.h"
#include "pattern/Singleton.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <exception>
#include <functional>

NXS_NAMESPACE {
    template<typename _ResourceType>
    class ResourceManager : public Singleton<ResourceManager<_ResourceType>>
    {
    public:
        /**
         * Get or create a resource from @c path .
         * Return @c nullptr if the resource can't be created or doesn't exist.
         */
        Ref<_ResourceType> get(const std::string& path)
        {
            size_t resourceId = std::hash<std::string> {} (path);
            Ref<_ResourceType> result = get(resourceId);

            if (!result) {
                result = Ref<_ResourceType>(create(path));
                if (result) {
                _resources.emplace(resourceId, result);
                }
            }

            return result;
        }
        
        /**
         * Get a resource from the @c resourceId .
         * Return @c nullptr if the resource doesn't exist.
         */
        Ref<_ResourceType> get(uint64_t resourceId)
        {
            auto itr = _resources.find(resourceId);
            return (itr != _resources.end()) ? itr->second : nullptr;
        }
    protected:
        /**
         * Create a resource from the specified @c path .
         */
        virtual Ref<_ResourceType> create(const std::string& path)
        {
            throw std::runtime_error("ResourceManager::create is not implemented!");
            return Ref<_ResourceType>();
        }
    private:
        std::unordered_map<uint64_t, Ref<_ResourceType>> _resources;
    };
}

#endif // __NXS_RESOURCE_MANAGER_H__
