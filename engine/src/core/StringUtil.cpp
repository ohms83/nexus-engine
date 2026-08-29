#include "nexus/core/StringUtil.h"
#include "nexus/debug/LogDispatcher.h"

#include <locale>     // For std::tolower, std::toupper with locale
#include <algorithm>  // For std::transform, std::find_if, std::find_if_not
#include <string>
#include <vector>
#include <map>
#include <string_view>
#include <format>

// C++20 ranges
#include <iostream>
#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(StrUtil);

// Helper function for character conversion (locale-independent if possible)
char StrUtil::ToLowerChar(const char c)
{
    // Use static locale to avoid recreation overhead if performance is critical,
    // otherwise std::tolower works fine. For ASCII/common usage, ::tolower is simpler.
    return CAST<char>(std::tolower(CAST<unsigned char>(c)));
}

char StrUtil::ToUpperChar(const char c)
{
    return CAST<char>(std::toupper(CAST<unsigned char>(c)));
}

bool StrUtil::IsWhitespace(const char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string StrUtil::TrimLeft(const std::string_view original, const std::string_view delimiters)
{
    // C++17 string_view::find_first_not_of
    const size_t firstChar = original.find_first_not_of(delimiters);
    if (std::string_view::npos == firstChar)
    {
        // All characters are delimiters, return empty string
        return "";
    }
    return std::string(original.substr(firstChar));
}

std::string StrUtil::TrimRight(const std::string_view original, const std::string_view delimiters)
{
    // C++17 string_view::find_last_not_of
    const size_t lastChar = original.find_last_not_of(delimiters);
    if (std::string_view::npos == lastChar)
    {
        // All characters are delimiters, return empty string
        return "";
    }
    // Substr needs length, which is (lastChar_index - firstChar_index + 1)
    return std::string(original.substr(0, lastChar + 1));
}

std::string StrUtil::Trim(const std::string_view original, const std::string_view delimiters)
{
    // Chains the trim functions.
    // No need for try-catch for these std::string_view operations.
    return TrimLeft(TrimRight(original, delimiters), delimiters);
}

std::string& StrUtil::Replace(std::string& src, const std::string_view subStr, const std::string_view newSubStr)
{
    if (subStr.empty()) return src;
    if (const size_t index = src.find(subStr); index != std::string::npos)
    {
        src.replace(index, subStr.size(), newSubStr);
    }
    return src;
}

std::string StrUtil::Replace(const std::string_view src, const std::string_view subStr, const std::string_view newSubStr)
{
    // Create a copy, then call the modifying version
    std::string result(src);
    StrUtil::Replace(result, subStr, newSubStr); // Explicitly qualify StrUtil
    return result;
}

std::string& StrUtil::ReplaceAll(std::string& src, const std::string_view subStr, const std::string_view newSubStr)
{
    if (subStr.empty()) return src;
    // Use a loop with find and replace
    size_t pos = 0;
    while ((pos = src.find(subStr, pos)) != std::string::npos)
    {
        src.replace(pos, subStr.length(), newSubStr);
        pos += newSubStr.length(); // Advance past the newly inserted substring
    }
    return src;
}

std::string StrUtil::ReplaceAll(const std::string_view src, const std::string_view subStr, const std::string_view newSubStr)
{
    // Create a copy, then call the modifying version
    std::string result(src);
    StrUtil::ReplaceAll(result, subStr, newSubStr);
    return result;
}

std::string& StrUtil::ReplaceAll(std::string& src, const std::map<std::string, std::string>& replaceMapping)
{
    for (const auto& [fst, snd] : replaceMapping)
    {
        // Use the string_view overloads for efficiency if map keys/values are temporary or can be used as such
        StrUtil::ReplaceAll(src, fst, snd);
    }
    return src;
}

std::string StrUtil::ReplaceAll(const std::string_view src, const std::map<std::string, std::string>& replaceMapping)
{
    std::string result(src);
    StrUtil::ReplaceAll(result, replaceMapping);
    return result;
}

std::string& StrUtil::ToLower(std::string& str)
{
    // std::transform works well. Using the static helper for character conversion.
    std::ranges::transform(str, str.begin(), StrUtil::ToLowerChar);
    return str;
}

std::string StrUtil::ToLower(const std::string_view str)
{
    std::string copy(str); // Create a copy from string_view
    StrUtil::ToLower(copy); // Modify the copy
    return copy; // Return the modified copy (RVO/NRVO apply)
}

std::string& StrUtil::ToUpper(std::string& str)
{
    std::ranges::transform(str, str.begin(), StrUtil::ToUpperChar);
    return str;
}

std::string StrUtil::ToUpper(const std::string_view str)
{
    std::string copy(str);
    StrUtil::ToUpper(copy);
    return copy;
}

std::vector<std::string> StrUtil::Split(const std::string_view str, const std::string_view delimiters)
{
    std::vector<std::string> tokens;
    if (str.empty()) {
        return tokens;
    }
    if (delimiters.empty()) {
        return {str.data()};
    }

    size_t index = 0;
    size_t count = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (delimiters.find(str[i]) == std::string::npos) {
            ++count;
            continue;
        }
        // count == 0 means we found the delimeter itself.
        if (count != 0) {
            tokens.emplace_back(std::string(str.substr(index, count)));
        }
        index = i+1;
        count = 0;
    }
    if (count != 0) {
        tokens.emplace_back(std::string(str.substr(index, count)));
    }

    return tokens;
}

std::vector<std::string> StrUtil::Split(std::string_view str, char delimiter)
{
    std::vector<std::string> tokens;
    if (str.empty())
    {
        return tokens;
    }

    for (const auto& segment : str | std::views::split(delimiter) | std::views::filter([](auto sv){ return !sv.empty(); }))
    {
        tokens.emplace_back(segment.begin(), segment.end());
    }
    return tokens;
}

std::optional<bool> StrUtil::ParseBool(const std::string_view str)
{
    const std::vector<std::string> trueStrings = {
        "true", "on", "yes", "1"
    };
    const std::vector<std::string> falseStrings = {
        "false", "off", "no", "0"
    };
    const std::string normalizedStr = ToLower(str);
    if (std::ranges::find(trueStrings, normalizedStr) != trueStrings.end()) return true;
    if (std::ranges::find(falseStrings, normalizedStr) != falseStrings.end()) return false;
    LOG_WARNING(LogStrUtil, std::format("Failed to parse the string '{}' as a boolean", str));
    return std::nullopt;
}
