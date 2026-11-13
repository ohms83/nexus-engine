#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/container/Array.h"
#include "nexus/geom/Box.h"
#include "nexus/geom/Frustum.h"

#include "SceneNode.h"

NXS_NAMESPACE
{
    struct NodeBound
    {
        WeakRef<SceneNode> node;
        Box bound;

        Identifier Id() const {
            auto nodeRef = node.lock();
            return nodeRef ? nodeRef->GetId() : NXS_INVALID_ID;
        }

        bool operator == (const NodeBound& rhs) const {
            const auto id = Id();
            return id != NXS_INVALID_ID && id == rhs.Id();
        }

        bool operator < (const NodeBound& rhs) const {
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
    };
}