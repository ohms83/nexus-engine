//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/RenderingInterface.h>

NXS_NAMESPACE
{
    class GLRenderingInterface final : public RenderingInterface
    {
    public:
        GLRenderingInterface(WindowContext* window, const GraphicsConfig& config);
        virtual ~GLRenderingInterface();
    };
}
