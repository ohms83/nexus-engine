//
// Created by nutta on 7/10/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include <vector>

NXS_NAMESPACE
{
    //! A fast hashing algorithm. NOTE: This doesn't suit for cryptography.
    class Hasher final
    {
    public:
        Hasher() = default;
        explicit Hasher(uint32 seed);

        uint32 Hash32(const void* data, size_t size) const;
        uint32 Hash32(const std::vector<uint8>& buffer) const;

    private:
        uint32 m_seed = 0;
    };
}
