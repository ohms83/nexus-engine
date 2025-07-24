#pragma once

#include <nexus/NxsDefine.h>
#include "Transform.h"

#include <string>

NXS_NAMESPACE
{
    class SceneNode
    {
    public:
        SceneNode() = default;
        explicit SceneNode(const std::string& name);
        virtual ~SceneNode() = default;

        glm::mat4 GetMatrix() const
        {
            return transform.GetMatrix();
        }

        const std::string& GetName() const
        {
            return m_name;
        }

        Transform transform;

    protected:
        std::string m_name;
    };
}