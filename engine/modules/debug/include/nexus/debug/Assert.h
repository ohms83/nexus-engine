#pragma once

#include <cassert>
#include <string>
#include <format>

#include "sigslot/signal.hpp"
#include "nexus/NxsDefine.h"

#if !defined(NXS_NO_LOG)
#define NXS_ASSERT(Condition) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("({}:{}) {}", __FILE__, __LINE__, #Condition); \
            nxs::Assert::Check(false, formatted); \
        } \
    } while (0)

/**
 * @brief Custom assertion macro that logs an error message and terminates the program if the condition is false.
 * 
 * @param Condition The condition to evaluate.
 * @param Message The message to log if the assertion fails.
 */
#define NXS_ASSERT_MSG(Condition, Message) \
    do { \
        if (!(Condition)) { \
            const auto formatted = std::format("({}:{}) {}. {}", __FILE__, __LINE__, #Condition, Message); \
            nxs::Assert::Check(false, formatted); \
        } \
    } while (0)
#else
#define NXS_ASSERT(Condition) do {} while (0)
#define NXS_ASSERT_MSG(Condition, Message) do {} while (0)
#endif // !defined(NXS_NO_LOG)
    
namespace nxs
{
    class NXS_API Assert
    {
    public:
        /**
         * @brief Checks the given condition and logs an error message if it is false.
         * 
         * @param condition The condition to check.
         * @param message The message to log if the condition is false.
         */
        static void Check(bool condition, const std::string& message);

        /**
         * @brief Signal emitted when an assertion fails. The signal carries the formatted error message.
         */
        static sigslot::signal<const std::string&> onAssertionFailedEvent;
    };
}