#pragma once

#include "nexus/NxsDefine.h"
#include "mimalloc.h"

#include <unordered_map>

NXS_NAMESPACE
{
    template <typename TKey, typename TValue>
    class HashMap : public std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>
    {
    public:
        using Base = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>;
        using Base::Base;

        HashMap() = default;
        HashMap(std::initializer_list<typename Base::value_type> init)
            : Base(init)
        {
        }
    };
}
