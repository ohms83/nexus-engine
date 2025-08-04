#pragma once

#include "nexus/NxsDefine.h"

#include <map>
#include <string>
#include <sstream>
#include <vector>

NXS_NAMESPACE
{
    /**
     * @brief Utility class for common string manipulation operations.
     * @details The @c StrUtil class provides a collection of static methods for performing
     * various string operations such as trimming whitespace, replacing substrings,
     * converting a case, and splitting strings.
     *
     * @note This class assumes ASCII or single-byte character sets for case conversion and
     * whitespace handling. For Unicode support, more advanced libraries would be required.
     * @ingroup Core
     */
    class StrUtil
    {
    public:
        /**
         * @brief Trims leading characters from a string based on a set of delimiters.
         * @param original The input string to trim.
         * @param delimiters A string_view containing characters to be trimmed from the left.
         * Defaults to common whitespace characters (" \t\n\r").
         * @return A new @c std::string with leading delimiters removed. Returns an empty string
         * if the original string contains only delimiters or is empty.
         * @code
         * std::string s = "   Hello World";
         * std::string trimmed = nxs::StrUtil::TrimLeft(s); // "Hello World"
         *
         * std::string s2 = "---Value---";
         * std::string trimmed2 = nxs::StrUtil::TrimLeft(s2, "-"); // "Value---"
         * @endcode
         */
        static std::string TrimLeft(std::string_view original, std::string_view delimiters = " \t\n\r");

        /**
         * @brief Trims trailing characters from a string based on a set of delimiters.
         * @param original The input string to trim.
         * @param delimiters A string_view containing characters to be trimmed from the right.
         * Defaults to common whitespace characters (" \t\n\r").
         * @return A new @c std::string with trailing delimiters removed. Returns an empty string
         * if the original string contains only delimiters or is empty.
         * @code
         * std::string s = "Hello World   ";
         * std::string trimmed = nxs::StrUtil::TrimRight(s); // "Hello World"
         *
         * std::string s2 = "---Value---";
         * std::string trimmed2 = nxs::StrUtil::TrimRight(s2, "-"); // "---Value"
         * @endcode
         */
        static std::string TrimRight(std::string_view original, std::string_view delimiters = " \t\n\r");

        /**
         * @brief Trims both leading and trailing characters from a string based on a set of delimiters.
         * @param original The input string to trim.
         * @param delimiters A string_view containing characters to be trimmed from both ends.
         * Defaults to common whitespace characters (" \t\n\r").
         * @return A new @c std::string with leading and trailing delimiters removed.
         * @code
         * std::string s = "   Hello World   ";
         * std::string trimmed = nxs::StrUtil::Trim(s); // "Hello World"
         *
         * std::string s2 = "---Value---";
         * std::string trimmed2 = nxs::StrUtil::Trim(s2, "-"); // "Value"
         * @endcode
         */
        static std::string Trim(std::string_view original, std::string_view delimiters = " \t\n\r");

        /**
         * @brief Replaces the first occurrence of a substring within a string (in-place).
         * @param src The @c std::string to modify. Its content will be changed.
         * @param subStr The substring to search for.
         * @param newSubStr The substring to replace @c subStr with.
         * @return A reference to the modified @c src string.
         * @code
         * std::string s = "hello world";
         * nxs::StrUtil::Replace(s, "world", "galaxy"); // s is now "hello galaxy"
         * @endcode
         */
        static std::string& Replace(std::string& src, std::string_view subStr, std::string_view newSubStr);

        /**
         * @brief Replaces the first occurrence of a substring within a string (new string).
         * @param src The input string_view. Its content remains unchanged.
         * @param subStr The substring to search for.
         * @param newSubStr The substring to replace @c subStr with.
         * @return A new @c std::string with the first occurrence replaced.
         * @code
         * std::string_view s_view = "hello world";
         * std::string new_s = nxs::StrUtil::Replace(s_view, "world", "galaxy"); // new_s is "hello galaxy"
         * // s_view remains "hello world"
         * @endcode
         */
        static std::string Replace(std::string_view src, std::string_view subStr, std::string_view newSubStr);

        /**
         * @brief Replaces all occurrences of a substring within a string (in-place).
         * @param src The @c std::string to modify. Its content will be changed.
         * @param subStr The substring to search for.
         * @param newSubStr The substring to replace @c subStr with.
         * @return A reference to the modified @c src string.
         * @code
         * std::string s = "banana banana banana";
         * nxs::StrUtil::ReplaceAll(s, "banana", "apple"); // s is now "apple apple apple"
         * @endcode
         */
        static std::string& ReplaceAll(std::string& src, std::string_view subStr, std::string_view newSubStr);

        /**
         * @brief Replaces all occurrences of a substring within a string (new string).
         * @param src The input string_view. Its content remains unchanged.
         * @param subStr The substring to search for.
         * @param newSubStr The substring to replace @c subStr with.
         * @return A new @c std::string with all occurrences replaced.
         * @code
         * std::string_view s_view = "apple pie, apple sauce";
         * std::string new_s = nxs::StrUtil::ReplaceAll(s_view, "apple", "peach"); // new_s is "peach pie, peach sauce"
         * // s_view remains "apple pie, apple sauce"
         * @endcode
         */
        static std::string ReplaceAll(std::string_view src, std::string_view subStr, std::string_view newSubStr);

        /**
         * @brief Replaces all occurrences of multiple substrings based on a mapping (in-place).
         * @param src The @c std::string to modify. Its content will be changed.
         * @param replaceMapping A map where keys are substrings to find and values are their replacements.
         * @return A reference to the modified @c src string.
         * @note Replacements are applied sequentially based on the map's iteration order.
         * If replacement A -> B and then B -> C are in the map, and the original string
         * has A, it might become C. Consider the order carefully.
         * @code
         * std::string s = "Hello [USER]!";
         * std::map<std::string, std::string> mapping = {{"[USER]", "Alice"}, {"!", "?"}};
         * nxs::StrUtil::ReplaceAll(s, mapping); // s is now "Hello Alice?"
         * @endcode
         */
        static std::string& ReplaceAll(std::string& src, const std::map<std::string, std::string>& replaceMapping);

        /**
         * @brief Replaces all occurrences of multiple substrings based on a mapping (new string).
         * @param src The input string_view. Its content remains unchanged.
         * @param replaceMapping A map where keys are substrings to find and values are their replacements.
         * @return A new @c std::string with all occurrences replaced based on the mapping.
         * @code
         * std::string_view s_view = "Item: [ITEM_NAME], Price: [PRICE]";
         * std::map<std::string, std::string> mapping = {{"[ITEM_NAME]", "Laptop"}, {"[PRICE]", "1200"}};
         * std::string new_s = nxs::StrUtil::ReplaceAll(s_view, mapping); // new_s is "Item: Laptop, Price: 1200"
         * // s_view remains "Item: [ITEM_NAME], Price: [PRICE]"
         * @endcode
         */
        static std::string ReplaceAll(std::string_view src, const std::map<std::string, std::string>& replaceMapping);

        /**
         * @brief Converts all characters in a string to lowercase (in-place).
         * @param str The @c std::string to modify. Its content will be changed.
         * @return A reference to the modified @c str string.
         * @code
         * std::string s = "HeLlO WoRlD";
         * nxs::StrUtil::ToLower(s); // s is now "hello world"
         * @endcode
         */
        static std::string& ToLower(std::string& str);

        /**
         * @brief Converts all characters in a string to lowercase (new string).
         * @param str The input string_view. Its content remains unchanged.
         * @return A new @c std::string with all characters converted to lowercase.
         * @code
         * std::string_view s_view = "MiXeD CaSe";
         * std::string lower_s = nxs::StrUtil::ToLower(s_view); // lower_s is "mixed case"
         * // s_view remains "MiXeD CaSe"
         * @endcode
         */
        static std::string ToLower(std::string_view str);

        /**
         * @brief Converts all characters in a string to uppercase (in-place).
         * @param str The @c std::string to modify. Its content will be changed.
         * @return A reference to the modified @c str string.
         * @code
         * std::string s = "Hello world";
         * nxs::StrUtil::ToUpper(s); // s is now "HELLO WORLD"
         * @endcode
         */
        static std::string& ToUpper(std::string& str);

        /**
         * @brief Converts all characters in a string to uppercase (new string).
         * @param str The input string_view. Its content remains unchanged.
         * @return A new @c std::string with all characters converted to uppercase.
         * @code
         * std::string_view s_view = "Mixed Case";
         * std::string upper_s = nxs::StrUtil::ToUpper(s_view); // upper_s is "MIXED CASE"
         * // s_view remains "Mixed Case"
         * @endcode
         */
        static std::string ToUpper(std::string_view str);

        /**
         * @brief Splits a string into a vector of substrings based on multiple delimiters.
         * @param str The input string_view to split.
         * @param delimiters A string_view containing characters that act as delimiters.
         * @return A @c std::vector of @c std::string objects, where each string is a token
         * separated by any of the specified delimiters.
         * @code
         * std::string_view data = "alpha,beta;gamma,delta";
         * std::vector<std::string> tokens = nxs::StrUtil::Split(data, ",;");
         * // tokens will contain {"alpha", "beta", "gamma", "delta"}
         * @endcode
         */
        static std::vector<std::string> Split(std::string_view str, std::string_view delimiters);

        /**
         * @brief Splits a string into a vector of substrings based on a single character delimiter.
         * @details An overload for convenience when only a single character delimiter is needed.
         * Leverages C++20's @c <ranges> library.
         * @param str The input string_view to split.
         * @param delimiter The single character to use as a delimiter.
         * @return A @c std::vector of @c std::string objects, where each string is a token
         * separated by the delimiter.
         * @code
         * std::string_view csv = "header1,header2,header3";
         * std::vector<std::string> headers = nxs::StrUtil::Split(csv, ',');
         * // headers will contain {"header1", "header2", "header3"}
         * @endcode
         */
        static std::vector<std::string> Split(std::string_view str, char delimiter);

    private:
        // Internal helper for locale-independent char to lower conversion
        static char ToLowerChar(char c);
        // Internal helper for locale-independent char to upper conversion
        static char ToUpperChar(char c);
        // Internal helper for whitespace check
        static bool IsWhitespace(char c);
    };
}
