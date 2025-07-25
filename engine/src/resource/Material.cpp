//
// Created by nutta on 7/12/2025.
//

#include <nexus/resource/Material.h>

USING_NAMESPACE_NXS;

Material::Material(const uint32 resourceId)
    : Resource(resourceId)
{
}

uint8* Material::Load_Impl(const std::string& path, size_t& out_size)
{
    return nullptr;
}
