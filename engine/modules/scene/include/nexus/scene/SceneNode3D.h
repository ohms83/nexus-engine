#pragma once

#include "nexus/NxsDefine.h"

#include "SceneNode.h"
#include "component/TransformComponent.h"

#include <string>

NXS_NAMESPACE
{
    /**
     * 
     */
    class SceneNode3D : public SceneNode
    {
    public:
        SceneNode3D() = delete;
        explicit SceneNode3D(Ref<entt::registry> registry, std::string  name = "");
        ~SceneNode3D() override = default;

        IMPLEMENT_NODE(SceneNode3D);
    };
}