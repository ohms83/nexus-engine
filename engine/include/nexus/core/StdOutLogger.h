//
// Created by nutta on 8/2/2025.
//

#pragma once

#include "Logger.h"

NXS_NAMESPACE
{
    class StdOutLogger final : public ILogger
    {
    public:
        void Log(const std::string& message) override;
        void Flush() override;
    };
}