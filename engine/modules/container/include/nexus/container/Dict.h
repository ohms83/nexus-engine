#pragma once

#include "nexus/NxsDefine.h"
#include "mimalloc.h"

#include <map>

NXS_NAMESPACE
{
    template <typename TKey, typename TValue>
    class Dict : public std::map<TKey, TValue, std::less<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>
    {
    public:
        using Base = std::map<TKey, TValue, std::less<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>;
        using Base::Base;

        Dict() = default;
        Dict(std::initializer_list<typename Base::value_type> init)
            : Base(init)
        {
        }
    };
}
