//
// Created by nutta on 7/18/2025.
//

#include <format>
#include <iostream>
#include <ostream>
#include <nexus/core/Logger.h>

USING_NAMESPACE_NXS;

#define LOG_MSG(Type, Category, Message, callback) \
    const auto formatted = std::format("[{}][{}] {}\n", #Type, Category, Message); \
    if (IsFlagSet(LogToFile)) m_logFile << formatted; \
    if (IsFlagSet(LogToStdOut)) std::cout << formatted; \
    callback(formatted);

void Logger::Init(const int32 initFlags)
{
    Logger& instance = Instance();
    instance.m_flags = initFlags;

    if (instance.IsFlagSet(LogToFile))
    {
        instance.OpenLogFile();
    }
}

void Logger::Log(const std::string& category, const std::string& message)
{
    // const auto formatted = std::format("[Log][{}] {}\n", category, message);
    // if (IsFlagSet(LogToFile)) m_logFile << formatted;
    // if (IsFlagSet(LogToStdOut)) std::cout << formatted;
    LOG_MSG(Log, category, message, logCallback);
}

void Logger::Warning(const std::string& category, const std::string& message)
{
    LOG_MSG(Warning, category, message, warningCallback);
}

void Logger::Error(const std::string& category, const std::string& message)
{
    LOG_MSG(Error, category, message, errorCallback);
}

void Logger::Assert(const std::string& category, const std::string& message)
{
    LOG_MSG(Assert, category, message, assertCallback);
}

Logger::Logger()
{
    m_logPath = std::format("{}/logs/{}", NXS_PROJECT_PATH, m_logName);
}

Logger::~Logger()
{
    if (IsFlagSet(LogToFile)) CloseLogFile();
}

void Logger::OpenLogFile()
{
    // TODO: Support multiple log files.
    m_logFile.open(m_logPath, std::ios::out);
}

void Logger::CloseLogFile()
{
    m_logFile.close();
}
