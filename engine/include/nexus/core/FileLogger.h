//
// Created by nutta on 8/2/2025.
//
#pragma once

#include "Logger.h"
#include <fstream>

NXS_NAMESPACE
{
    class FileLogger final : public ILogger
    {
    public:
        explicit FileLogger(const std::string& fileName);
        ~FileLogger() override;

        void Log(LogLevel level, const std::string& message) override;
        void Flush() override;
    private:
        std::string m_logName = "debug.log";
        std::string m_logPath;
        std::fstream m_logFile;
    };
}
