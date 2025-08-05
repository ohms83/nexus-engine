//
// Created by nutta on 8/2/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

#include <fstream>
#include <format>
#include <string>
#include <set>

#include "Logger.h"
#include "Task.h"

#define DECLARE_LOG_EXTERN(LogCategory) extern const std::string Log##LogCategory
#define DEFINE_LOG(LogCategory) const std::string Log##LogCategory = #LogCategory

#if !defined(NO_LOG)
#define ENABLE_LOG(LogCategory) nxs::LogDispatcher::Instance().EnableCategory(LogCategory, true)
#define DISABLE_LOG(LogCategory) nxs::LogDispatcher::Instance().EnableCategory(LogCategory, false)

#define LOG_DEBUG(Category, Message) nxs::LogDispatcher::Instance().Debug(Category, Message)
#define LOG_INFO(Category, Message) nxs::LogDispatcher::Instance().Info(Category, Message)
#define LOG_WARNING(Category, Message) nxs::LogDispatcher::Instance().Warning(Category, Message)
#define LOG_ERROR(Category, Message) \
    do { \
        const auto formatted = std::format("({}:{}) {}", __FILE__, __LINE__, Message); \
        nxs::LogDispatcher::Instance().Error(Category, formatted); \
    } while(0);

#define LOG_FATAL(Category, Message) \
    do { \
        const auto formatted = std::format("({}:{}) {}", __FILE__, __LINE__, Message); \
        nxs::LogDispatcher::Instance().Fatal(Category, formatted); \
    } while(0);

#define NXS_ASSERT(Condition) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("Assertion Failed! ({}:{}) {}", __FILE__, __LINE__, #Condition); \
            nxs::LogDispatcher::Instance().Log(LogLevel::Fatal, "Assert", formatted); \
            nxs::LogDispatcher::Destroy(); \
        } \
        assert(Condition); \
    } while(0);

#define NXS_ASSERT_MSG(Condition, Message) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("Assertion Failed! ({}:{}) {}. {}", __FILE__, __LINE__, #Condition, Message); \
            nxs::LogDispatcher::Instance().Log(LogLevel::Fatal, "Assert", formatted); \
            nxs::LogDispatcher::Destroy(); \
        } \
        assert(Condition); \
    } while(0);
#else
#define ENABLE_LOG(LogCategory)
#define DISABLE_LOG(LogCategory)

#define LOG_DEBUG(Category, Message)
#define LOG_INFO(Category, Message)
#define LOG_WARNING(Category, Message)
#define LOG_ERROR(Category, Message)
#define LOG_FATAL(Category, Message)
#define NXS_ASSERT(Condition)
#define NXS_ASSERT_MSG(Condition, Message)
#endif // !defined(NO_LOG)

NXS_NAMESPACE
{
    class LogDispatcher final
    {
    public:
        LogDispatcher() = default;
        ~LogDispatcher() = default;

        static void Init();
        static void Destroy();
        static LogDispatcher& Instance();

        void AddLogger(const Ref<ILogger>& logger);
        void AddLoggers(std::initializer_list<Ref<ILogger>> loggers);
        void Flush() const;

        void Log(LogLevel level, const std::string& category, const std::string& message);

        void Debug(const std::string& category, const std::string& message);
        void Info(const std::string& category, const std::string& message);
        void Warning(const std::string& category, const std::string& message);
        void Error(const std::string& category, const std::string& message);
        /**
         * A fatal erro has occurred and the application cannot continue.
         * This will forcefully terminate the app.
         */
        void Fatal(const std::string& category, const std::string& message);

        /**
         * Set the minimum log level and discard all the log below the specified level.
         * @param level Minimum log level. For example, if this parameter is set to @c LogLevel::Warning,
         * any logs from @c LogLevel::Debug and @c LogLevel::Info will be discarded.
         */
        void SetMinumumLogLevel(LogLevel level);
        //! Enable/disable logs from a specific category.
        void EnableCategory(const std::string& category, bool enable);

        const std::string& Message() const
        {
            return m_message;
        }
    private:
        std::vector<Ref<ILogger>> m_loggers;
        std::string m_message;

        LogLevel m_minimumLogLevel = LogLevel::Debug;
        //! A set of disabled log categories.
        std::set<std::string> m_disableLogs;

        TaskHandle m_flushTask{};
    };
}