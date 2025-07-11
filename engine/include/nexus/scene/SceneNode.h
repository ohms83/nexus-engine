#pragma once

#include <nexus/NxsDefine.h>
#include "Transform.h"

NXS_NAMESPACE
{
    class SceneNode
    {
    public:
        SceneNode() = default;
        virtual ~SceneNode() = default;

        glm::mat4 GetMatrix() const
        {
            return transform.GetMatrix();
        }

        Transform transform;
    };
}