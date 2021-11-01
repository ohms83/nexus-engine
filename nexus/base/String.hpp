#ifndef __NXS_STRING_H__
#define __NXS_STRING_H__

#include "NxsDefine.h"

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
        
        /**
         *  Find the first occurence of the given @c subStr and replace it with @c newSubStr. If the sub-string to be replaced cannot be found,
         *  the source string will be kept unchange.
         *  @return A constant reference of @c src.
         */
        static const std::string& replace(std::string& src, const std::string& subStr, const std::string& newSubStr);
        
        /**
         *  Find all occurences of the given @c subStr and replace them with @c newSubStr. If the sub-string to be replaced cannot be found,
         *  the source string will be kept unchange.
         *  @return A constant reference of @c src.
         */
        static const std::string& replaceAll(std::string& src, const std::string& subStr, const std::string& newSubStr);
        
        /**
         *  Transform all characters in the given string into lower case.
         *  @return A constant reference of @c str.
         */
        static const std::string& tolower(std::string& str);
        
        /**
         *  Transform all characters in the given string into upper case.
         *  @return A constant reference of @c str.
         */
        static const std::string& toupper(std::string& str);

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
        
        /// Concatenate strings from @c stringList and put the specified @c separator between them.
        static std::string join( const std::vector<std::string>& stringList, const std::string& separator );

        static const std::string DELIMITERS;
    };
}

#endif // __NXS_STRING_H__
