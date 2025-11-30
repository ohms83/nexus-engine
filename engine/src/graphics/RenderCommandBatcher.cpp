#include "nexus/graphics/RenderCommandBatcher.h"
USING_NAMESPACE_NXS;

void RenderCommandBatcher::Batch(std::vector<RenderCommand>& commands)
{
    if (commands.empty()) return;
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
            if (src.modelMatrix) dst.AddInstance(src.modelMatrix);
        }
        else
        {
            commands[write++] = std::move(commands[read]);
        }
    }
    commands.resize(write);
}
