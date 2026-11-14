#include "nexus/scene/partition/Quadtree.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/geom/Collider.h"

#include <algorithm>
#include <span>

#include "Remotery.h"

USING_NAMESPACE_NXS;

Quadtree::Quadtree(uint32_t maxDepth, uint32_t partitionSize, const Box& worldBound)
    : m_bound(worldBound)
    , m_maxDepth(maxDepth)
    , m_partitionSize(partitionSize)
{
    m_nodeList.reserve(m_partitionSize);
}

void Quadtree::AddNode(Ref<SceneNode> node, const Box& bound)
{
    rmt_ScopedCPUSample(Quadtree_AddNode, 0)
    if (!node || !IsOverlap(bound)) return;

    if (!m_quadrants.empty())
    {
        for (const auto& quadrant : m_quadrants)
        {
            quadrant->AddNode(node, bound);
        }
        return;
    }

    const auto id = node->GetId();
    const auto it = std::ranges::find_if(m_nodeList, [id](const PartitionEntry& n) {
        return n.Id() == id;
    });
    if (it != m_nodeList.end()) {
        it->bound = bound;
        return;
    }

    if (m_depth >= m_maxDepth || m_nodeList.size() < m_partitionSize)
    {
        m_nodeList.emplace_back(PartitionEntry {
            node,
            bound
        });
        return;
    }

    // Leaf capacity reached and depth < maxDepth: add node and subdivide
    m_nodeList.emplace_back(PartitionEntry {
        node,
        bound
    });

    Subdivide();
}

void Quadtree::Subdivide()
{
    rmt_ScopedCPUSample(Quadtree_Subdivide, 0)
    const auto& center = m_bound.center;
    const auto& extent = m_bound.extent;
    const glm::vec3 childExtent(extent.x * 0.5f, extent.y, extent.z * 0.5f);
    const std::array<Box, NUM_QUADRANT> bounds = {
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

    const auto newDepth = m_depth + 1;
    m_quadrants.reserve(NUM_QUADRANT);
    for (int i = 0; i < NUM_QUADRANT; ++i)
    {
        auto treeNode = std::make_shared<Quadtree>(m_maxDepth, m_partitionSize, bounds[i]);
        treeNode->m_depth = newDepth;
        m_quadrants.push_back(treeNode);
    }

    for (const auto& item : m_nodeList)
    {
        if (auto itemNode = item.node.lock())
        {
            for (const auto& quadrant : m_quadrants)
            {
                quadrant->AddNode(itemNode, item.bound);
            }
        }
    }

    m_nodeList.clear();
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
    std::erase_if(m_nodeList, [id](const PartitionEntry& node) {
        return node.Id() == id;
    });

    for (const auto& quadrant : m_quadrants)
    {
        quadrant->RemoveNode(id);
    }
}

void Quadtree::RemoveAllNodes()
{
    m_nodeList.clear();
    for (auto& treeNode : m_quadrants)
    {
        treeNode->RemoveAllNodes();
    }
    m_quadrants.clear();
}

void Quadtree::QueryInternal(const Frustum& frustum, Array<SceneNode*>& results) const
{
    rmt_ScopedCPUSample(Quadtree_QueryInternal, 0)
    if (!Collider::IsIntersect(m_bound, frustum)) {
        return;
    }

    for (const auto& node : m_nodeList)
    {
        if (auto nodeRef = node.node.lock())
        {
            if (nodeRef->IsActive() && Collider::IsIntersect(node.bound, frustum))
            {
                results.push_back(nodeRef.get());
            }
        }
    }

    for (const auto& treeNode : m_quadrants)
    {
        treeNode->QueryInternal(frustum, results);
    }
}

void Quadtree::Query(const Frustum& frustum, Array<SceneNode*>& results) const
{
    rmt_ScopedCPUSample(Quadtree_Query, 0)
    if (!Collider::IsIntersect(m_bound, frustum)) return;

    const size_t startIndex = results.size();
    QueryInternal(frustum, results);

    if (m_depth == 0 && results.size() > startIndex)
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