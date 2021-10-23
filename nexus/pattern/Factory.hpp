#ifndef __NXS_FACTORY_H__
#define __NXS_FACTORY_H__

#include "NxsDefine.h"

#include <map>
#include <functional>
#include <sstream>

// #include "engine/util/TypeCheck.h"

NXS_NAMESPACE {
    template<typename _Index, typename _Class>
    class Factory
    {
    public:
        /// Register factory function
        void registerCreator(_Index index, std::function<_Class*()> creator)
        {
            if (_factories.find(index) != _factories.end())
            {
                // std::stringstream ss;
                // ss << "A factory function is already register for INDEX=" << index;
                // LOG_WARNING("%s", ss.str().c_str());
                return;
            }

            _factories.emplace(index, creator);
        }

        _Class* create(_Index index) const
        {
            const auto& itr = _factories.find(index);
            if (itr == _factories.end())
            {
                // std::stringstream ss;
                // ss << "Cannot find a factory function for INDEX=" << index;
                // LOG_WARNING("%s", ss.str().c_str());
                return nullptr;
            }

            return itr->second();
        }

    private:
        std::map<_Index, std::function<_Class*()>> _factories;
    };

    /// A specialized template of @c Factory that using @c std::string as indexes
    template<typename _Class>
    class FactoryString : public Factory<std::string, _Class>
    {

    };
}

#define REGISTER_CREATOR_FUNC(_factory, _discreetClass, _baseClass) { \
    _factory.registerCreator(#_discreetClass, []() -> _baseClass* { return new (std::nothrow) _discreetClass(); } ); \
}

#endif // __NXS_FACTORY_H__
