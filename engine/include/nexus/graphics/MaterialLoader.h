//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/ResourceLoader.h"

#include "Material.h"

NXS_NAMESPACE
{
    class MaterialLoader final : public IResourceLoader
    {
    public:
        Ref<Resource> Load(const std::string& path, uint32 id) override;
    };
}
