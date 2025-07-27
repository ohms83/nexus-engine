//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "entt/entity/registry.hpp"
#include "nexus/NxsDefine.h"
#include "nexus/scene/SceneNode.h"

NXS_NAMESPACE
{
    void MoveNode(entt::registry& registry, float dt);
    void RotateNode(entt::registry& registry, float dt);
}