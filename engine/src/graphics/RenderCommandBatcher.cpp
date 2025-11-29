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
