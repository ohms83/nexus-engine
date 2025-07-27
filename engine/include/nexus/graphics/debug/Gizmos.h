//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/Color.h"

NXS_NAMESPACE
{
    class RenderSystem;
    class Camera;

    class Gizmos
    {
    public:
        static void Init(const  RenderSystem& renderSystem);
        static void CleanUp();
        static void DrawPoint(RenderSystem& renderSystem,
            const glm::vec3& position,
            const Color3F& color,
            const glm::mat4& view,
            const glm::mat4& projection,
            float size = 10);
    };
}
