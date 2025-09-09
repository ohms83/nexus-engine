//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/Color.h"

NXS_NAMESPACE
{
    class RenderSystem;

    class Gizmos
    {
    public:
        static void Init(const  RenderSystem& renderSystem);
        static void Destroy();
        //! Clear gizmos data from the last frame.
        static void Clear();
        static void ProcessDraw(RenderSystem& renderSystem, const glm::mat4& cameraMtx);
        static void DrawPoint(
            RenderSystem& renderSystem,
            const glm::vec3& position,
            const Color3F& color,
            float size = 10);
        static void DrawLine(
            RenderSystem& renderSystem,
            const glm::vec3& start,
            const glm::vec3& end,
            const Color3F& color);
        static void DrawOutlineSphere(
            RenderSystem& renderSystem,
            const glm::vec3& position,
            float radius,
            uint32_t = 36,
            //! Thye color of Latitude-line along x-y plane
            const Color3F& xyLatColor = Color3F::Red,
            //! Thye color of Latitude-line along y-z plane
            const Color3F& yzLatColor = Color3F::Green,
            //! Longtitude-line's color
            const Color3F& longColor = Color3F::Yellow);
    };
}
