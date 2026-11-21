//
// Created by nutta on 8/2/2025.
//

#pragma once

#include "Logger.h"
#include "nexus/NxsDefine.h"

namespace nxs
{
    class NXS_API StdOutLogger final : public ILogger
    {
    public:
        void Log(LogLevel level, const std::string& message) override;
        void Flush() override;
    };
}