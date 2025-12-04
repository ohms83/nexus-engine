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
        MaterialManager(Ref<RenderingInterface> renderingInterface);

        Ref<Material> GetDefaultMaterial() const
        {
            return m_defaultMaterial;
        }

    private:
        Ref<Material> m_defaultMaterial;
    };
}
