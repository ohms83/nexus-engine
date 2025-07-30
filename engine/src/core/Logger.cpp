//
// Created by nutta on 7/18/2025.
//

#include <format>
#include <iostream>
#include <ostream>
#include <nexus/core/Logger.h>

#include "core/TaskManager.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Logger);

#define LOG_FORMAT(Level, Category, Message) std::format("[{}][{}] {}\n", #Level, Category, Message);

Logger* Logger::m_instance = new Logger();

void Logger::Init(const int32 initFlags, const float flushInterval)
{
    Logger& instance = Instance();
    instance.m_flags = initFlags;
    instance.OpenLogFile();
    instance.SetFlushInterval(flushInterval);
}

void Logger::Destroy()
{
    delete m_instance;
}

void Logger::Log(LogLevel level, const std::string& category, const std::string& message)
{
    if (level < m_minimumLogLevel || m_disableLogs.contains(category)) return;

    std::string formatted;
    switch (level)
    {
    case LogLevel::Debug:
        formatted = LOG_FORMAT(Debug, category, message);
        break;
    case LogLevel::Warning:
        formatted = LOG_FORMAT(Warning, category, message);
        break;
    case LogLevel::Error:
        formatted = LOG_FORMAT(Error, category, message);
        break;
    case LogLevel::Fatal:
        formatted = LOG_FORMAT(Fatal, category, message);
        break;
    default:
        formatted = LOG_FORMAT(Info, category, message);
        break;
    }
    if (IsFlagSet(LogToFile)) m_logFile << formatted;
    if (IsFlagSet(LogToStdOut)) std::cout << formatted;
    m_message += formatted;
    logCallback(level, formatted);
}

void Logger::Debug(const std::string& category, const std::string& message)
{
    Log(LogLevel::Debug, category, message);
}

void Logger::Info(const std::string& category, const std::string& message)
{
    Log(LogLevel::Info, category, message);
}

void Logger::Warning(const std::string& category, const std::string& message)
{
    Log(LogLevel::Warning, category, message);
}

void Logger::Error(const std::string& category, const std::string& message)
{
    Log(LogLevel::Error, category, message);
}

void Logger::Fatal(const std::string& category, const std::string& message)
{
    Log(LogLevel::Fatal, category, message);
    Disconnect();
    CloseLogFile();
    assert(false);
}

void Logger::Flush()
{
    if (IsFlagSet(LogToFile)) m_logFile << std::flush;
    if (IsFlagSet(LogToStdOut)) std::cout << std::flush;
}

void Logger::SetFlushInterval(const float seconds)
{
    auto& taskManager = TaskManager::Instance();
    taskManager.StopTask(m_flushTask);
    m_flushTask = taskManager.CreateTask(
        // Flush action
        [&] { Flush(); },
        // Repeat counts
        -1,
        // Delay
        0,
        // Task repeat interval
        seconds,
        // Run immediately
        true);
}

void Logger::SetMinumumLogLevel(const LogLevel level)
{
    m_minimumLogLevel = level;
}

void Logger::EnableCategory(const std::string& category, bool enable)
{
    if (!enable) m_disableLogs.insert(category);
    else m_disableLogs.erase(category);
}

void Logger::Disconnect()
{
    Info(LogLogger, "Disconnect all delegates");
    logCallback.disconnect_all();
}

Logger::Logger()
{
    m_logPath = std::format("{}/logs/{}", NXS_PROJECT_PATH, m_logName);
}

Logger::~Logger()
{
    Disconnect();
    CloseLogFile();
}

void Logger::OpenLogFile()
{
    if (!IsFlagSet(LogToFile)) return;
    // TODO: Support multiple log files.
    std::cout << "Opening log file: " << m_logPath << std::endl;
    m_logFile.open(m_logPath, std::ios::out);
}

void Logger::CloseLogFile()
{
    if (!IsFlagSet(LogToFile)) return;
    m_logFile.close();
}
