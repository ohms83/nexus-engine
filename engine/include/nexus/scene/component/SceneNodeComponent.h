//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    struct SceneNodeComponent
    {
        Identifier id = 0;
        std::string name;
        bool active = true;
    };
}
