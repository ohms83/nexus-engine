#include "nexus/scene/partition/Quadtree.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/geom/Collider.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <span>

#include "Remotery.h"

USING_NAMESPACE_NXS;

Quadtree::Quadtree(uint32_t maxDepth, uint32_t partitionSize, const Box& worldBound)
    : m_bound(worldBound)
    , m_maxDepth(maxDepth)
    , m_partitionSize(partitionSize)
{
    size_t totalNodes = 1;
    size_t levelNodes = 1;
    for (uint32_t d = 0; d < m_maxDepth; ++d)
    {
        levelNodes *= 4;
        totalNodes += levelNodes;
    }

    m_tree.resize(totalNodes);
    m_tree[0].create = true;
    m_tree[0].depth = 0;
    m_tree[0].quadrant = Quadrant::None;
    m_tree[0].bound = worldBound;
    m_tree[0].nodeList.reserve(m_partitionSize);
}

void Quadtree::AddNode(Ref<SceneNode> node, const Box& bound)
{
    rmt_ScopedCPUSample(Quadtree_AddNode, 0)
    if (!node || !IsOverlap(bound)) return;
    if (m_tree.empty()) return;

    const auto id = node->GetId();

    std::array<uint32_t, 128> stack;
    int top = 0;
    stack[top++] = 0;

    while (top > 0)
    {
        uint32_t currIndex = stack[--top];
        auto& curr = m_tree[currIndex];

        if (!curr.quadrants.empty())
        {
            uint32_t firstChild = 4 * currIndex + 1;
            for (uint32_t q = 0; q < 4; ++q)
            {
                uint32_t childIndex = firstChild + q;
                if (childIndex < m_tree.size() && m_tree[childIndex].create)
                {
                    if (Collider::IsIntersect(m_tree[childIndex].bound, bound))
                    {
                        stack[top++] = childIndex;
                    }
                }
            }
        }
        else
        {
            auto it = std::ranges::find_if(curr.nodeList, [id](const PartitionEntry& n) {
                return n.Id() == id;
            });
            if (it != curr.nodeList.end())
            {
                it->bound = bound;
                continue;
            }

            curr.nodeList.emplace_back(PartitionEntry {
                node,
                bound
            });

            if (curr.depth < static_cast<int32_t>(m_maxDepth) && curr.nodeList.size() > m_partitionSize)
            {
                Subdivide(currIndex);
            }
        }
    }
}

void Quadtree::Subdivide(uint32_t nodeIndex)
{
    rmt_ScopedCPUSample(Quadtree_Subdivide, 0)
    if (nodeIndex >= m_tree.size()) return;
    auto& node = m_tree[nodeIndex];
    if (node.depth >= static_cast<int32_t>(m_maxDepth)) return;
    if (!node.quadrants.empty()) return;

    uint32_t firstChild = 4 * nodeIndex + 1;
    if (firstChild + 3 >= m_tree.size()) return;

    const auto& center = node.bound.center;
    const auto& extent = node.bound.extent;
    const glm::vec3 childExtent(extent.x * 0.5f, extent.y, extent.z * 0.5f);
    const std::array<Box, 4> bounds = {
        // PlusXPlusZ
        Box {
            glm::vec3 { center.x + childExtent.x, center.y, center.z + childExtent.z },
            childExtent
        },
        // MinusXPlusZ
        Box {
            glm::vec3 { center.x - childExtent.x, center.y, center.z + childExtent.z },
            childExtent
        },
        // MinusXMinusZ
        Box {
            glm::vec3 { center.x - childExtent.x, center.y, center.z - childExtent.z },
            childExtent
        },
        // PlusXMinusZ
        Box {
            glm::vec3 { center.x + childExtent.x, center.y, center.z - childExtent.z },
            childExtent
        }
    };

    node.quadrants.clear();
    node.quadrants.reserve(4);
    for (uint32_t i = 0; i < 4; ++i)
    {
        uint32_t childIndex = firstChild + i;
        auto& child = m_tree[childIndex];
        child.create = true;
        child.depth = node.depth + 1;
        child.quadrant = static_cast<Quadrant>(i);
        child.bound = bounds[i];
        child.nodeList.clear();
        child.nodeList.reserve(m_partitionSize);
        child.quadrants.clear();
        node.quadrants.push_back(&child);
    }

    auto items = std::move(node.nodeList);
    node.nodeList.clear();

    for (const auto& item : items)
    {
        if (auto itemNode = item.node.lock())
        {
            for (uint32_t i = 0; i < 4; ++i)
            {
                uint32_t childIndex = firstChild + i;
                if (Collider::IsIntersect(m_tree[childIndex].bound, item.bound))
                {
                    m_tree[childIndex].nodeList.push_back(item);
                }
            }
        }
    }

    for (uint32_t i = 0; i < 4; ++i)
    {
        uint32_t childIndex = firstChild + i;
        if (m_tree[childIndex].depth < static_cast<int32_t>(m_maxDepth) &&
            m_tree[childIndex].nodeList.size() > m_partitionSize)
        {
            Subdivide(childIndex);
        }
    }
}

void Quadtree::Subdivide(int32_t depth, Quadrant quadrant)
{
    rmt_ScopedCPUSample(Quadtree_Subdivide, 0)
    if (depth <= 0 || quadrant == Quadrant::None)
    {
        Subdivide(0);
        return;
    }
    for (size_t i = 0; i < m_tree.size(); ++i)
    {
        if (m_tree[i].create && m_tree[i].depth == depth && m_tree[i].quadrant == quadrant)
        {
            Subdivide(static_cast<uint32_t>(i));
            return;
        }
    }
}

void Quadtree::UpdateNode(Identifier id, const Box& bound)
{
    rmt_ScopedCPUSample(Quadtree_UpdateNode, 0)
    RemoveNode(id);
    if (auto* sceneNode = SceneNode::FindNode(id))
    {
        AddNode(sceneNode->GetSelf(), bound);
    }
}

void Quadtree::RemoveNode(Identifier id)
{
    rmt_ScopedCPUSample(Quadtree_RemoveNode, 0)
    for (auto& node : m_tree)
    {
        if (node.create && !node.nodeList.empty())
        {
            std::erase_if(node.nodeList, [id](const PartitionEntry& entry) {
                return entry.Id() == id;
            });
        }
    }
}

void Quadtree::RemoveAllNodes()
{
    rmt_ScopedCPUSample(Quadtree_RemoveAllNodes, 0)
    for (auto& node : m_tree)
    {
        node.create = false;
        node.depth = -1;
        node.quadrant = Quadrant::None;
        node.nodeList.clear();
        node.quadrants.clear();
    }

    if (!m_tree.empty())
    {
        m_tree[0].create = true;
        m_tree[0].depth = 0;
        m_tree[0].quadrant = Quadrant::None;
        m_tree[0].bound = m_bound;
        m_tree[0].nodeList.reserve(m_partitionSize);
    }
}

void Quadtree::QueryInternal(const Frustum& frustum, Array<SceneNode*>& results) const
{
    rmt_ScopedCPUSample(Quadtree_QueryInternal, 0)
    if (m_tree.empty() || !m_tree[0].create) return;
    if (!Collider::IsIntersect(m_tree[0].bound, frustum)) return;

    std::array<uint32_t, 128> stack;
    int top = 0;
    stack[top++] = 0;

    while (top > 0)
    {
        uint32_t currIndex = stack[--top];
        const auto& node = m_tree[currIndex];

        for (const auto& item : node.nodeList)
        {
            if (auto itemNode = item.node.lock())
            {
                if (itemNode->IsActive() && Collider::IsIntersect(item.bound, frustum))
                {
                    results.push_back(itemNode.get());
                }
            }
        }

        if (!node.quadrants.empty())
        {
            uint32_t firstChild = 4 * currIndex + 1;
            for (uint32_t q = 0; q < 4; ++q)
            {
                uint32_t childIndex = firstChild + q;
                if (childIndex < m_tree.size() && m_tree[childIndex].create)
                {
                    if (Collider::IsIntersect(m_tree[childIndex].bound, frustum))
                    {
                        stack[top++] = childIndex;
                    }
                }
            }
        }
    }
}

void Quadtree::Query(const Frustum& frustum, Array<SceneNode*>& results) const
{
    rmt_ScopedCPUSample(Quadtree_Query, 0)
    if (!Collider::IsIntersect(m_bound, frustum)) return;

    const size_t startIndex = results.size();
    QueryInternal(frustum, results);

    if (results.size() > startIndex)
    {
        auto range = std::span(results.data() + startIndex, results.size() - startIndex);
        std::ranges::sort(range);
        auto [first, last] = std::ranges::unique(range);
        const auto newSize = startIndex + std::distance(range.begin(), first);
        results.resize(newSize);
    }
}

bool Quadtree::IsOverlap(const Box& bound) const
{
    rmt_ScopedCPUSample(Quadtree_IsOverlap, 0)
    return Collider::IsIntersect(m_bound, bound);
}
