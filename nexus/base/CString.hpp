//
//  CString.hpp
//  Nexus
//
//  Created by nuttachai on 4/11/21.
//

#pragma once

#include "NxsDefine.h"

#include <vector>
#include <functional>

NXS_NAMESPACE {
    /**
     * A utility class for manipulating C-style string (const char*).
     * This class is separated from @c String to make it clear that it only handles
     * C string and not C++ @c std::string.
     *
     * Most of the functions here theoretically work faster than their @c String counterparts
     * because there's no dynamic memory allocation involve. However, they are more destructive
     * because they directly modify the source string. Please use it with care.
     */
    class CString
    {
    public:
        /**
         * Parse the given string, replace all occurence of line-feed and newline characters
         * with null-terminator, and return a pointer to the next line, or @c nullptr,
         * if it reaches the end of the string.
         *
         * @note No new object will be created and the caller is still responsible for releasing
         *       memory of the given @c str.
         *
         * @warning This function is destructive: it writes the '\0' characters in the elements of the string @c str
         */
        static char* getLine(char* str);
        
        /**
         * Parse the given string, replace all occurence of line-feed and newline characters
         * with null-terminator and return a list of pointers to every lines in the given string.
         *
         * @note No new object will be created and the caller is still responsible for releasing
         *       memory of the given @c str.
         *
         * @warning This function is destructive: it writes the '\0' characters in the elements of the string @c str
         */
        static std::vector<char*> getLines(char* str);
        
        /**
         * Iterate the given string line-by-line to perform the specified task.
         * @param task A callback function that accepts a pointer to the current line as a parameter.
         *             Return @c false, to prematurely stop the iteration.
         *
         * @warning This function is destructive: it writes the '\0' characters in the elements of the string @c str
         */
        static void iterateLines(char* str, std::function<bool(char*)> task);
        
        /**
         * Split string into tokens.
         * @param joinDelim If @c true, this function will behave like @c strtok where all the consecutive delimeters
         *                  will be treated as one; for example, CString::split("Hello//World", "/", true) will return
         *                  ["Hello", "World"]; on the other hand, ["Hello", "", "World"] will be returned if the parameter
         *                  is @c false.
         *
         * @warning This function is destructive: it writes the '\0' characters in the elements of the string @c str
         */
        static std::vector<char*> split(char* str, const char* delimeters, bool joinDelim = false);
        
        static char* trim(char* str, const char* delimeters = DELIMITERS);
        /**
         * Skip all the leading characters in string.
         * @param str The input string.
         * @param delimeters All the leading chracters present in this list will be skipped.
         * @return The pointer to the first character in @c str that is not a part of @c delimeters.
         *         If the entire string consists of delimeters, the return value will point to the null-terminator.
         */
        static char* trimLeft(char* str, const char* delimeters = DELIMITERS);
        /**
         * Remove all the trailing characters from string.
         * @param str The input string.
         * @param delimeters All the trailing chracters present in this list will be removed.
         * @return The same pointer as @c str.
         *
         * @warning This function is destructive: it writes the '\0' characters in the elements of the string @c str
         */
        static char* trimRight(char* str, const char* delimeters = DELIMITERS);
        
        static const char DELIMITERS[];
    };
}
