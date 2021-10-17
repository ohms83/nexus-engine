#pragma once
#include "nxsMacros.h"

#include <string>
#include <sstream>
#include <exception>
#include <vector>
#include <utility>

NXS_NAMESPACE {
    /**
     * This static class provides useful string manipulation functions and is never meant to be a custom string class.
     * Since @c std::string is already well optimized and maintained, let's not reinvent the wheel.
     */
    class String
    {
    public:
        static std::string trim(const std::string& original, const std::string delimeters = DELIMITERS);
        static std::string trimLeft(const std::string& original, const std::string delimeters = DELIMITERS);
        static std::string trimRight(const std::string& original, const std::string delimeters = DELIMITERS);

        /// Splits string into tokens.
        static std::vector<std::string> split(const std::string& str, const std::string& delimeters);

        /**
         *  Parse the given string and return its value in specified type.
         *  @param outValue An in/out parameter containing parsing result. In case of parsing error, this parameter won't be altered.
         *  @return @c true if there's no parsing error; otherwise, @c false
         */
        template<typename T>
        static bool parse(const std::string& str, T& outValue)
        {
            try {
                std::stringstream ss;
                ss << str;
                ss >> outValue;
                return true;
            }
            catch(std::exception e) {
                return false;
            }
        }

        static const std::string DELIMITERS;
    };
}