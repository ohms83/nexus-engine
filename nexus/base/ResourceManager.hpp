#ifndef __NXS_RESOURCE_MANAGER_H__
#define __NXS_RESOURCE_MANAGER_H__

#include "nxsMacros.h"
#include "pattern/Singleton.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <exception>

NXS_NAMESPACE {
    template<typename _ResourceType>
    class ResourceManager : public Singleton<ResourceManager<_ResourceType>>
    {
    public:
        /**
         * Get or create a resource from @c path .
         * Return @c nullptr if the resource cant' be created or doesn't exist.
         */
        std::shared_ptr<_ResourceType> get(const std::string& path)
        {
            std::shared_ptr<_ResourceType> result;

            auto itr = _resources.find(path);
            if (itr == _resources.end()) {
                result = std::shared_ptr<_ResourceType>(create(path));
                _resources.emplace(path, result);
            }
            else {
                result = itr->second;
            }

            return result;
        }
    protected:
        /**
         * Create a resource from the specified @c path .
         */
        virtual _ResourceType* create(const std::string& path) const
        {
            throw std::runtime_error("ResourceManager::create is not implemented!");
            return nullptr;
        }
    private:
        std::unordered_map<std::string, std::shared_ptr<_ResourceType>> _resources;
    };
}

#endif // __NXS_RESOURCE_MANAGER_H__