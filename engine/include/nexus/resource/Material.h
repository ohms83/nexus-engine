//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Resource.h"
#include "Texture.h"
#include "nexus/graphics/Color.h"

NXS_NAMESPACE
{
    class Material final : public Resource
    {
    public:
        explicit Material(std::string path, uint32 resourceId);

        Color3F diffuse;
        Color3F specular;
        Color3F emissive;
        float shininess = 0;

        Ref<Texture> diffuseMap;
        Ref<Texture> specularMap;
        Ref<Texture> normalMap;
    };
}
