//
// Created by nutta on 8/2/2025.
//
#include "nexus/debug/LogDispatcher.h"

#include <format>
#include <memory>

using namespace nxs;

DEFINE_LOG(LogDispatcher);
DEFINE_LOG(Temp);

#define LOG_FORMAT(Level, Category, Message) std::format("[{}][{}] {}\n", #Level, Category, Message);

static Ptr<LogDispatcher> s_instance;

LogDispatcher::LogDispatcher()
{
    m_assertConnection = Assert::onAssertionFailedEvent.connect([this](const std::string& message) {
        Log(LogLevel::Fatal, "Assert", message);
        Destroy();
    });
}

LogDispatcher::~LogDispatcher()
{
    m_assertConnection.disconnect();
    m_loggers.clear();
}

void LogDispatcher::Init()
{
    s_instance = std::make_unique<LogDispatcher>();
}

void LogDispatcher::Destroy()
{
    s_instance.reset();
}

LogDispatcher& LogDispatcher::Instance()
{
    if (!s_instance) Init();
    return *s_instance;
}

void LogDispatcher::AddLogger(Ref<ILogger> logger)
{
    m_loggers.push_back(logger);
}

void LogDispatcher::AddLoggers(const std::initializer_list<Ref<ILogger>> loggers)
{
    for (auto& logger : loggers)
    {
        m_loggers.push_back(logger);
    }
}

void LogDispatcher::Flush() const
{
    for (const auto& logger : m_loggers) logger->Flush();
}

void LogDispatcher::Log(const LogLevel level, const std::string& category, const std::string& message)
{
    if (m_loggers.empty()) return;
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

    for (const auto& logger : m_loggers) logger->Log(level, formatted);

    m_message += formatted;
}

void LogDispatcher::Debug(const std::string& category, const std::string& message)
{
    Log(LogLevel::Debug, category, message);
}

void LogDispatcher::Info(const std::string& category, const std::string& message)
{
    Log(LogLevel::Info, category, message);
}

void LogDispatcher::Warning(const std::string& category, const std::string& message)
{
    Log(LogLevel::Warning, category, message);
}

void LogDispatcher::Error(const std::string& category, const std::string& message)
{
    Log(LogLevel::Error, category, message);
}

void LogDispatcher::Fatal(const std::string& category, const std::string& message)
{
    Log(LogLevel::Fatal, category, message);
    m_loggers.clear();
    assert(false);
}

void LogDispatcher::SetMinumumLogLevel(LogLevel level)
{
    m_minimumLogLevel = level;
}

void LogDispatcher::EnableCategory(const std::string& category, bool enable)
{
    if (!enable) m_disableLogs.insert(category);
    else m_disableLogs.erase(category);
}
