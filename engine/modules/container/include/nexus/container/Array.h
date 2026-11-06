#pragma once

#include "nexus/NxsDefine.h"
#include "mimalloc.h"

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
}
