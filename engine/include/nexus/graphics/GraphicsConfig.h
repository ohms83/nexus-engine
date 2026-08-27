//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/serialize/IniParser.h"
#include "GraphicsConst.h"

NXS_NAMESPACE
{
    struct GraphicsConfig
    {
        GraphicsAPI api = GraphicsAPI::Undefined;
        int32_t screenWidth = 0;
        int32_t screenHeight = 0;
        //! Enable/disable VSync.
        bool vsync = false;

        bool LoadConfig(const std::string& filePath);
    };
}
