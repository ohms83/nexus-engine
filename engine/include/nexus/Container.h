#pragma once

#include "nexus/NxsDefine.h"
#include "mimalloc.h"

#include <vector>
#include <map>
#include <unordered_map>

NXS_NAMESPACE
{
    template <typename T>
    class Array : public std::vector<T, mi_stl_allocator<T>>
    {
    public:
        using Super = std::vector<T, mi_stl_allocator<T>>;
        using Super::Super;

        Array() = default;
        Array(std::initializer_list<T> init)
            : Super(init, mi_stl_allocator<T>())
        {
        }
    };

    template <typename TKey, typename TValue>
    class Dict : public std::map<TKey, TValue, std::less<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>
    {
    public:
        using Super = std::map<TKey, TValue, std::less<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>;
        using Super::Super;

        Dict() = default;
        Dict(std::initializer_list<typename Super::value_type> init)
            : Super(init)
        {
        }
    };

    template <typename TKey, typename TValue>
    class HashMap : public std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>
    {
    public:
        using Super = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, mi_stl_allocator<std::pair<const TKey, TValue>>>;
        using Super::Super;

        HashMap() = default;
        HashMap(std::initializer_list<typename Super::value_type> init)
            : Super(init)
        {
        }
    };
}