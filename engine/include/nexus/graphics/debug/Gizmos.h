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
        static void GenerateDrawCommands(RenderSystem& renderSystem, const glm::mat4& cameraMtx);
        static void DrawPoint(RenderSystem& renderSystem,
            const glm::vec3& position,
            const Color3F& color,
            float size = 10);
    };
}
