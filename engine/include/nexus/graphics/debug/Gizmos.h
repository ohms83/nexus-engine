//
// Created by nutta on 7/26/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/core/Color.h"
#include "nexus/graphics/RenderCommand.h"

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
        static void CreateRenderCommands(std::vector<RenderCommand>& outCommands, RenderSystem& renderSystem);
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
        static void DrawLocalAxes(
            RenderSystem& renderSystem,
            const glm::vec3& position,
            const glm::quat& rotation = glm::quat(1, 0, 0, 0),
            float axisLength = 1,
            const std::array<Color3F, 3>& axisColors = {Color3F::Red, Color3F::Green, Color3F::Blue});
        static void DrawOutlineBox(
            RenderSystem& renderSystem,
            const glm::vec3& center,
            const glm::vec3& extent,
            const glm::mat4& transform = glm::mat4(1),
            const Color3F& color = Color3F::Red);
        static void DrawOutlineSphere(
            RenderSystem& renderSystem,
            const glm::vec3& position,
            float radius,
            const glm::mat4& transform = glm::mat4(1),
            uint32_t numSegments = 36,
            //! Thye color of Latitude-line along x-y plane
            const Color3F& xyLatColor = Color3F::Red,
            //! Thye color of Latitude-line along y-z plane
            const Color3F& yzLatColor = Color3F::Green,
            //! Longtitude-line's color
            const Color3F& longColor = Color3F::Yellow);
    };
}
