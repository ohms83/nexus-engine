//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"

#include "Material.h"
#include "MaterialLoader.h"

#include "nexus/core/ResourceManager.h"

NXS_NAMESPACE
{
    class MaterialManager final : public ResourceManager<Material>
    {
    public:
        MaterialManager()
        {
            RegisterLoader(std::make_unique<MaterialLoader>());
        }
    };
}
