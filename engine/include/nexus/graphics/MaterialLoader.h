//
// Created by nutta on 7/12/2025.
//
#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/resource/ResourceLoader.h"

#include "Material.h"

NXS_NAMESPACE
{
    class MaterialLoader final : public IResourceLoader
    {
    public:
        explicit MaterialLoader(const Ref<RenderingInterface>& renderingInterface, ResourceManager& resourceManager);

        MAYBE_UNUSED Ref<Resource> Load(const std::string& path, uint32 id) override;
        MAYBE_UNUSED Ref<LoadResult> LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback) override { return nullptr;}

    private:
        Ref<RenderingInterface> m_renderingInterface;
        ResourceManager& m_resourceManager;
    };
}
