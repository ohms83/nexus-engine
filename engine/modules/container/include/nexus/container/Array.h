#pragma once

#include "nexus/NxsDefine.h"
#include "mimalloc.h"

#include <algorithm>
#include <concepts>
#include <ranges>
#include <type_traits>
#include <vector>

NXS_NAMESPACE
{
    template <typename T>
    class Array : public std::vector<T, mi_stl_allocator<T>>
    {
    public:
        using Base = std::vector<T, mi_stl_allocator<T>>;
        using Base::Base;

        Array() = default;
        Array(std::initializer_list<T> init)
            : Base(init, mi_stl_allocator<T>())
        {
        }
    };

    template <typename T, typename Range>
        requires std::ranges::input_range<Range> &&
                 (!std::same_as<std::remove_cvref_t<Range>, Array<T>>)
    bool operator==(const Array<T>& lhs, const Range& rhs)
    {
        return std::ranges::equal(lhs, rhs);
    }

    template <typename Range, typename T>
        requires std::ranges::input_range<Range> &&
                 (!std::same_as<std::remove_cvref_t<Range>, Array<T>>)
    bool operator==(const Range& lhs, const Array<T>& rhs)
    {
        return std::ranges::equal(lhs, rhs);
    }
}
