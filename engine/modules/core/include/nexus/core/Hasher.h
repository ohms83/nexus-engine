//
// Created by nutta on 7/10/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include <vector>
#include <string>

NXS_NAMESPACE
{
    //! A fast hashing algorithm. NOTE: This doesn't suit for cryptography.
    class Hasher final
    {
    public:
        Hasher() = default;
        explicit Hasher(uint32_t seed);

        uint32_t Hash32(const void* data, size_t size) const;
        uint32_t Hash32(const std::vector<uint8_t>& buffer) const;
        uint32_t Hash32(const std::string& str) const;

    private:
        uint32_t m_seed = 0;
    };
}
