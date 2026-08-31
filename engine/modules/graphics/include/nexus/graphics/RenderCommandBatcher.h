#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderCommand.h"

NXS_NAMESPACE
{
    class RenderCommandBatcher
    {
    public:
        // Batch adjacent commands with identical draw parameters to reduce state changes.
        static void Batch(std::vector<RenderCommand>& commands);
    };
}
