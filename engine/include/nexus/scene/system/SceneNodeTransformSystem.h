//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "entt/entt.hpp"

NXS_NAMESPACE
{
    void MoveNode(entt::registry& registry, float dt);
    void RotateNode(entt::registry& registry, float dt);
}