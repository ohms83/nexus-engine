#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/container/Array.h"
#include "nexus/geom/Box.h"
#include "nexus/geom/Frustum.h"

#include "SceneNode.h"

NXS_NAMESPACE
{
    struct PartitionEntry
    {
        WeakRef<SceneNode> node;
        Box bound;

        Identifier Id() const {
            auto nodeRef = node.lock();
            return nodeRef ? nodeRef->GetId() : NXS_INVALID_ID;
        }

        bool operator == (const PartitionEntry& rhs) const {
            const auto id = Id();
            return id != NXS_INVALID_ID && id == rhs.Id();
        }

        bool operator < (const PartitionEntry& rhs) const {
            const auto id = Id();
            return id != NXS_INVALID_ID && id < rhs.Id();
        }
    };

    class IScenePartition
    {
    public:
        virtual ~IScenePartition() = default;

        virtual void AddNode(Ref<SceneNode> node, const Box& bound) = 0;
        virtual void UpdateNode(Identifier id, const Box& bound) = 0;
        virtual void RemoveNode(Identifier id) = 0;
        virtual void RemoveAllNodes() = 0;
        virtual void Query(const Frustum& frustum, Array<SceneNode*>& results) const = 0;
        

        NODISCARD virtual uint32_t GetDepth() const = 0;
        NODISCARD virtual uint32_t GetMaxDepth() const = 0;
        NODISCARD virtual uint32_t GetPartitionSize() const = 0;
        NODISCARD virtual const Box& GetBound() const = 0;
        NODISCARD virtual size_t GetNodeCount() const = 0;
    };
}