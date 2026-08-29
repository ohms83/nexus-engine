//
// Created by nutta on 8/2/2025.
//

#pragma once

#include "Logger.h"

namespace nxs
{
    class StdOutLogger final : public ILogger
    {
    public:
        void Log(LogLevel level, const std::string& message) override;
        void Flush() override;
    };
}