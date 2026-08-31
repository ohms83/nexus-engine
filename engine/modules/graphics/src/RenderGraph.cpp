#include "nexus/graphics/RenderGraph.h"
USING_NAMESPACE_NXS;

std::vector<RenderPass> RenderGraph::Build(const std::vector<RenderPass>& passes)
{
    const size_t n = passes.size();
    std::vector<std::vector<size_t>> adj(n);
    std::vector<size_t> indeg(n, 0);

    // Build mapping from offscreen target name to writer pass index
    std::unordered_map<std::string, size_t> writerByName;
    for (size_t i = 0; i < n; ++i)
    {
        if (passes[i].targetType == RenderTargetType::Offscreen && !passes[i].offscreenTargetName.empty())
        {
            writerByName[passes[i].offscreenTargetName] = i;
        }
    }

    // Build edges: for each pass that reads a target, add edge from writer -> reader
    for (size_t i = 0; i < n; ++i)
    {
        for (const auto& t : passes[i].readTargets)
        {
            auto it = writerByName.find(t);
            if (it != writerByName.end())
            {
                const size_t writerIdx = it->second;
                adj[writerIdx].push_back(i);
                indeg[i]++;
            }
        }
    }

    // Kahn's algorithm: use priority as tie-breaker (lower priority first)
    struct Node { size_t idx; uint32_t prio; };
    auto cmp = [](const Node& a, const Node& b) { return a.prio > b.prio; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> zero(cmp);
    for (size_t i = 0; i < n; ++i) if (indeg[i] == 0) zero.push({i, passes[i].priority});

    std::vector<RenderPass> result;
    result.reserve(n);
    size_t visited = 0;
    while (!zero.empty())
    {
        const auto node = zero.top(); zero.pop();
        result.push_back(passes[node.idx]);
        visited++;
        for (auto v : adj[node.idx])
        {
            indeg[v]--;
            if (indeg[v] == 0) zero.push({v, passes[v].priority});
        }
    }

    if (visited < n)
    {
        // A cycle exists. Fallback to stable sort by priority.
        std::vector<RenderPass> sorted = passes;
        std::ranges::sort(sorted, std::ranges::less{}, &RenderPass::priority);
        return sorted;
    }

    return result;
}
