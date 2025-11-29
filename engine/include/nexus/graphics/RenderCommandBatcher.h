#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderCommand.h"

NXS_NAMESPACE
{
    class RenderCommandBatcher
    {
    public:
        // Batch adjacent commands with identical draw parameters to reduce state changes.
        static std::vector<RenderCommand> Batch(const std::vector<RenderCommand>& commands);
        // Batch commands in place without creating a new vector. This compacts the list.
        static void BatchInPlace(std::vector<RenderCommand>& commands);
    };
}
