//
// Created by nutta on 7/22/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "nexus/core/LogDispatcher.h"
#include "nexus/resource/Mesh.h"

NXS_NAMESPACE
{
    class PlaneMesh final : public Mesh
    {
    public:
        explicit PlaneMesh(uint32 resourceId);

        bool Load(const std::string& path) override
        {
            NXS_ASSERT(false);
            return true;
        }
    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override
        {
            return nullptr;
        }
    };
}
