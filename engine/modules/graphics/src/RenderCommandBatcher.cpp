#include "nexus/graphics/RenderCommandBatcher.h"
#include <span>
USING_NAMESPACE_NXS;

size_t RenderCommandBatcher::BatchImpl(std::span<RenderCommand> commands)
{
    if (commands.empty()) return 0;
    size_t write = 0;
    for (size_t read = 0; read < commands.size(); ++read)
    {
        if (write == 0)
        {
            commands[write++] = std::move(commands[read]);
            continue;
        }
        auto& dst = commands[write - 1];
        auto& src = commands[read];
        if (dst.CanBatchWith(src))
        {
            dst.AddInstance(src.modelMatrix);
        }
        else
        {
            commands[write++] = std::move(commands[read]);
        }
    }
    return write;
}
