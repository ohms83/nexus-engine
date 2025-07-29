//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <fstream>
#include <functional>
#include <string>
#include <ranges>
#include <format>
#include <set>
#include <nexus/NxsDefine.h>
#include <sigslot/signal.hpp>

#define DECLARE_LOG_EXTERN(LogCategory) extern const std::string Log##LogCategory
#define DEFINE_LOG(LogCategory) const std::string Log##LogCategory = #LogCategory

#define ENABLE_LOG(LogCategory) nxs::Logger::Instance().EnableCategory(LogCategory, true)
#define DISABLE_LOG(LogCategory) nxs::Logger::Instance().EnableCategory(LogCategory, false)

#define LOG_DEBUG(Category, Message) nxs::Logger::Instance().Debug(Category, Message)
#define LOG_INFO(Category, Message) nxs::Logger::Instance().Info(Category, Message)
#define LOG_WARNING(Category, Message) nxs::Logger::Instance().Warning(Category, Message)
#define LOG_ERROR(Category, Message) \
    do { \
        const auto formatted = std::format("({}:{}) {}", __FILE__, __LINE__, Message); \
        nxs::Logger::Instance().Error(Category, formatted); \
    } while(0);

#define LOG_FATAL(Category, Message) \
    do { \
        const auto formatted = std::format("({}:{}) {}", __FILE__, __LINE__, Message); \
        nxs::Logger::Instance().Fatal(Category, formatted); \
    } while(0);

#define NXS_ASSERT(Condition) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("Assertion Failed! ({}:{}) {}", __FILE__, __LINE__, #Condition); \
            nxs::Logger::Instance().Log(nxs::Logger::LogLevel::Fatal, "Assert", formatted); \
            nxs::Logger::Destroy(); \
        } \
        assert(Condition); \
    } while(0);

#define NXS_ASSERT_MSG(Condition, Message) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("Assertion Failed! ({}:{}) {}. {}", __FILE__, __LINE__, #Condition, Message); \
            nxs::Logger::Instance().Log(nxs::Logger::LogLevel::Fatal, "Assert", formatted); \
            nxs::Logger::Destroy(); \
        } \
        assert(Condition); \
    } while(0);

NXS_NAMESPACE
{
    class Logger
    {
    public:
        enum class LogLevel
        {
            /**
             * Mainly using for debug. The log will be stripped in the Release build, and
             * can contain sensitive info
             */
            Debug,
            /**
             * Used for logging crucial information that contains no sensitive information,
             * for example, system's configuration, or the current game state.
             */
            Info,
            /**
             * Logging ignorable errors that might need some attention.
             */
            Warning,
            /**
             * Logging crucial errors that can't be ignored.
             */
            Error,
            /**
             * A fatal error has occurred and the application cannot continue.
             */
            Fatal,
            Num
        };
        using LogCallback = sigslot::signal<LogLevel, const std::string&>;

        NODISCARD static Logger& Instance()
        {
            return *m_instance;
        }

        // Logger init flags
        static constexpr int32 LogToStdOut  = 0x01;
        static constexpr int32 LogToFile    = 0x02;

        static void Init(int32 initFlags);
        //! Destroy Logger's singleton.
        static void Destroy();

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

        const std::string& Message() const
        {
            return m_message;
        }

        void Flush();

        /**
         * Set the minimum log level and discard all the log below the specified level.
         * @param level Minimum log level. For example, if this parameter is set to @c LogLevel::Warning,
         * any logs from @c LogLevel::Debug and @c LogLevel::Info will be discarded.
         */
        void SetMinumumLogLevel(LogLevel level);
        //! Enable/disable logs from a specific category.
        void EnableCategory(const std::string& category, bool enable);


        //! Disconnect all registered delegates.
        void Disconnect();

        LogCallback logCallback;

    protected:
        std::string m_logName = "debug.log";
        std::string m_logPath;
        std::string m_message;
        int32 m_flags = LogToStdOut | LogToFile;
        std::fstream m_logFile;
        LogLevel m_minimumLogLevel = LogLevel::Debug;
        //! A set of disabled log categories.
        std::set<std::string> m_disableLogs;

    private:
        Logger();
        virtual ~Logger();

        NODISCARD bool IsFlagSet(const int32 flag) const
        {
            return (m_flags & flag) == flag;
        }

        void OpenLogFile();
        void CloseLogFile();

        static Logger* m_instance;
    };
}