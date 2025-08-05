#pragma once

#include "nexus/NxsDefine.h"

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <optional>

#include "nexus/core/LogDispatcher.h"

DECLARE_LOG_EXTERN(IniParser);

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

        /**
         * @brief Parses a string into a specified type using @c std::istringstream.
         *
         * @details This is a general-purpose parsing function that works for any type
         * that has an @c operator>> defined for @c std::istream. It performs robust error
         * checking, returning @c std::nullopt if the parsing fails or if the entire
         * string is not consumed (e.g., parsing "123abc" as an int).
         *
         * @tparam T The target type to parse into. Must support stream extraction (@c operator>> ).
         * @param str The input @c std::string_view to parse.
         * @return An @c std::optional<T> containing the parsed value on success, or
         * @c std::nullopt on any failure.
         *
         * @note When parsing a boolean type, this function relies on @c std::istringstream's
         * default behavior, which only recognizes the integer values "1" and "0" for @c true
         * and @c false respectively. For a more robust and human-readable parsing that
         * handles strings like "true", "false", "on", or "off", the dedicated
         * @c StrUtil::ParseBool function is a more preferable option.
         *
         * @code
         * // Success case
         * auto maybe_int = StrUtil::Parse<int>("12345");
         * if (maybe_int.has_value()) {
         * int val = *maybe_int; // val is 12345
         * }
         *
         * // Failure case (invalid format)
         * auto maybe_double = StrUtil::Parse<double>("123.45xyz");
         * if (!maybe_double) {
         * // maybe_double is std::nullopt
         * }
         * @endcode
         */
        template<typename T>
        static std::optional<T> Parse(const std::string_view str)
        {
            std::istringstream ss(str.data());
            T value;
            ss >> value;

            if (ss.fail() || ss.bad() || !ss.eof()) {
                // Failed to parse, or there's a leftover text in the stream
                LOG_WARNING(LogIniParser, std::format("Failed to parse the string '{}'", str));
                return std::nullopt;
            }
            return value;
        }
        /**
         * @brief Parses a string into a boolean value, supporting various string representations.
         *
         * @details This function performs a case-insensitive check against common boolean
         * string representations such as "true", "false", "on", "off", "yes", "no", "1", and "0".
         * It is a more robust alternative to `StrUtil::Parse<bool>` for parsing booleans
         * from a human-readable text.
         *
         * @param str The input @c std::string_view to parse.
         * @return An @c std::optional<bool> containing the parsed boolean value on success,
         * or @c std::nullopt if the string does not match any recognized boolean
         * representation.
         *
         * @code
         * // Success cases
         * auto maybe_true = StrUtil::ParseBool("on");     // returns std::optional<true>
         * auto maybe_false = StrUtil::ParseBool("False"); // returns std::optional<false>
         *
         * // Failure case
         * auto maybe_invalid = StrUtil::ParseBool("maybe"); // returns std::nullopt
         * @endcode
         */
        static std::optional<bool> ParseBool(std::string_view str);

        /**
         * @brief Parses a string into a numeric type using `std::from_chars`.
         *
         * @details This function is optimized for performance and is locale-independent,
         * making it ideal for parsing numerical data. It returns @c std::nullopt if the
         * string contains non-numeric characters, is out of range for the target type,
         * or is empty. Unlike @c Parse, it is very strict and does not handle leading
         * or trailing whitespace.
         *
         * @tparam T The target numeric type. Must be an @c std::integral or @c std::floating_point type.
         * @param str The input @c std::string_view to parse.
         * @return An @c std::optional<T> containing the parsed number on success, or
         * @c std::nullopt on any failure.
         *
         * @code
         * // Success case
         * auto maybe_long = StrUtil::ParseNumer<long long>("1234567890123");
         * if (maybe_long) {
         * long long val = maybe_long.value();
         * }
         *
         * // Failure case (out of range)
         * auto maybe_int = StrUtil::ParseNumer<int>("9999999999");
         * if (!maybe_int) {
         * // maybe_int is std::nullopt and an error is logged.
         * }
         * @endcode
         */
        template<typename T>
        requires std::integral<T> || std::floating_point<T>
        static std::optional<T> ParseNumer(const std::string_view str)
        {
            T value;
            const auto end = str.data() + str.size();
            auto [ptr, ec] = std::from_chars(str.data(), end, value);
            // Successfully parsed.
            if (ec == std::errc())
            {
                if (ptr == end) return value; // The string was fully parsed.
                LOG_WARNING(LogIniParser, std::format("'{}' partially contains non-numeric characters.", str));
                return std::nullopt;
            }

            if (ec == std::errc::invalid_argument) {
                LOG_ERROR(LogIniParser, std::format("'{}' is not a valid number.", str));
            } else if (ec == std::errc::result_out_of_range) {
                LOG_ERROR(LogIniParser, std::format("'{}' is too large for the designated number type.", str));
            }
            return std::nullopt;
        }

    private:
        // Internal helper for locale-independent char to lower conversion
        static char ToLowerChar(char c);
        // Internal helper for locale-independent char to upper conversion
        static char ToUpperChar(char c);
        // Internal helper for whitespace check
        static bool IsWhitespace(char c);
    };
}
