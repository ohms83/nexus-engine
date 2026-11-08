#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderCommand.h"

#include <span>

NXS_NAMESPACE
{
    class RenderCommandBatcher
    {
    public:
        // Batch adjacent commands with identical draw parameters to reduce state changes.
        template <typename Container>
        static void Batch(Container& commands)
        {
            const auto size = BatchImpl(std::span<RenderCommand>(commands.data(), commands.size()));
            commands.resize(size);
        }

    private:
        static size_t BatchImpl(std::span<RenderCommand> commands);
    };
}
