#pragma once

#include <nexus/NxsDefine.h>
#include <nexus/graphics/Color.h>
#include <nexus/math/Math.h>
#include "SceneNode.h"

NXS_NAMESPACE
{
    class Light : public SceneNode
    {
    public:
        Color3F color;
        float cutoffRange = NXS_INFINITE;
    };
}