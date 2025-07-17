//
// Created by nutta on 7/12/2025.
//
#pragma once

#include <nexus/NxsDefine.h>

#include "Resource.h"
#include "Texture.h"

NXS_NAMESPACE
{
    class Material final : public Resource
    {
    public:
        explicit Material(const uint32 hash);

        Color3F diffuse;
        Color3F specular;
        Color3F emissive;
        float shininess = 0;

        Ref<Texture> diffuseMap;
        Ref<Texture> specularMap;
        Ref<Texture> normalMap;

    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override;
    };
}
