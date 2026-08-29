//
// Created by nutta on 7/18/2025.
//

#pragma once

#include <string>

namespace nxs
{
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

    class ILogger
    {
    public:
        ILogger() = default;
        virtual ~ILogger() = default;

        virtual void Log(LogLevel level, const std::string& message) = 0;
        virtual void Flush() = 0;
    };
}