//
// Created by nutta on 7/10/2025.
//
#include <nexus/core/Hasher.h>

// Important: tells xxHash you're building it directly into your executable
#define XXH_INLINE_ALL
#define XXH_NO_XXH3
#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include "xxhash.h"

USING_NAMESPACE_NXS;

Hasher::Hasher(const uint32 seed)
    : m_seed(seed)
{}

uint32 Hasher::Hash32(const void* data, const size_t size) const
{
    return XXH32(data, size, m_seed);
}

uint32 Hasher::Hash32(const std::vector<uint8>& buffer) const
{
    return XXH32(buffer.data(), buffer.size() * sizeof(uint8), m_seed);
}
