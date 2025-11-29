#include "nexus/graphics/RenderCommandBatcher.h"
USING_NAMESPACE_NXS;

std::vector<RenderCommand> RenderCommandBatcher::Batch(const std::vector<RenderCommand>& commands)
{
    std::vector<RenderCommand> result;
    result.reserve(commands.size());

    for (const auto& cmd : commands)
    {
        if (!result.empty() && result.back().CanBatchWith(cmd))
        {
            // Merge instance
            auto& b = result.back();
            if (cmd.modelMatrix) b.AddInstance(cmd.modelMatrix);
            // Copy other fields as needed (none required for now)
        }
        else
        {
            result.push_back(cmd);
        }
    }

    return result;
}

void RenderCommandBatcher::BatchInPlace(std::vector<RenderCommand>& commands)
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
