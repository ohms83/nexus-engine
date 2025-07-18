//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <fstream>
#include <functional>
#include <string>
#include <ranges>
#include <nexus/NxsDefine.h>
#include <sigslot/signal.hpp>

NXS_NAMESPACE
{
    class Logger
    {
    public:
        // TODO: Replace with delegate
        using LogCallback = sigslot::signal<const std::string&>;

        static NODISCARD Logger& Instance()
        {
            static Logger instance;
            return instance;
        }

        // Logger init flags
        static constexpr int32 LogToStdOut  = 0x01;
        static constexpr int32 LogToFile    = 0x02;

        static void Init(int32 initFlags);

        void Log(const std::string& category, const std::string& message);
        void Warning(const std::string& category, const std::string& message);
        void Error(const std::string& category, const std::string& message);
        void Assert(const std::string& category, const std::string& message);

        LogCallback logCallback;
        LogCallback warningCallback;
        LogCallback errorCallback;
        LogCallback assertCallback;

    protected:
        std::string m_logName = "debug.log";
        std::string m_logPath;
        int32 m_flags = LogToStdOut | LogToFile;
        std::fstream m_logFile;

    private:
        Logger();
        virtual ~Logger();

        NODISCARD bool IsFlagSet(const int32 flag) const
        {
            return (m_flags & flag) == flag;
        }

        void OpenLogFile();
        void CloseLogFile();
    };
}