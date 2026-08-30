#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class Path
    {
    public:
        static std::string GetAssetPath(const std::string& filepath);
        static std::string GetEngineAssetPath(const std::string& filepath);
        static std::string GetGeneratedAssetPath(const std::string& filepath);
    };
}