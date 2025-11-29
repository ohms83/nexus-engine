#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderPass.h"
#include <vector>
#include <unordered_map>
#include <queue>

NXS_NAMESPACE
{
    /**
     * @brief Represents a render graph that organizes render passes based on their dependencies.
     * 
     */
    class RenderGraph
    {
    public:
        //! Build a topologically sorted list of render passes considering offscreen dependencies.
        //! If cycles are present, they will be ignored and priority order will be used as fallback.
        static std::vector<RenderPass> Build(const std::vector<RenderPass>& passes);
    };
}
