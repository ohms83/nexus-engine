//
// Created by nutta on 7/4/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "GraphicsConst.h"

NXS_NAMESPACE
{
    struct GraphicsConfig
    {
        GraphicsAPI api = GraphicsAPI::Undefined;
        int32 screenWidth = 0;
        int32 screenHeight = 0;
        //! Enable/disable VSync.
        bool vsync = false;
    };
}
