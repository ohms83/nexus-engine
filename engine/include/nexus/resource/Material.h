//
// Created by nutta on 7/12/2025.
//
#pragma once

#include <nexus/NxsDefine.h>

#include "Resource.h"
#include "Texture.h"

NXS_NAMESPACE
{
    class Material : public Resource
    {
    public:
        explicit Material(const uint32 hash);

        float diffuse = 0;
        float specular = 0;
        float emissive = 0;

        Texture* diffuseMap = nullptr;
        Texture* specularMap = nullptr;
        Texture* normalMap = nullptr;
    };
}
