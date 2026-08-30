//
// Created by nutta on 8/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/StringUtil.h"

#include <unordered_map>
#include <string>
#include <optional>

NXS_NAMESPACE
{
    class IniParser final
    {
    public:
        using Section = std::unordered_map<std::string, std::string>; // Map of key -> value
        using IniData = std::unordered_map<std::string, Section>;     // Map of section -> Section (key -> value)

        IniParser() = default;

        /**
         * @brief Loads and parses an INI file from the given path.
         * Clears any previously loaded data.
         * @param filePath The path to the INI file.
         * @return True if the file was successfully loaded and parsed, false otherwise.
         */
        bool Load(const std::string& filePath);

        /**
         * @brief Retrieves the value associated with a given key in a specific section.
         * Section and key names are case-insensitive.
         * @param sectionName The name of the section (e.g., "General").
         * Use an empty string "" for the global/default section.
         * @param keyName The name of the key (e.g., "LogLevel").
         * @return The found value as a string, or @c std::nullopt if not found.
         */
        std::optional<std::string> GetValue(const std::string& sectionName, const std::string& keyName) const;

        template<typename T>
        std::optional<T> GetValue(const std::string& sectionName, const std::string& keyName) const
        {
            if (const auto value = GetValue(sectionName, keyName); value.has_value()) {
                return StrUtil::Parse<T>(*value);
            }
            return std::nullopt;
        }

        std::optional<bool> GetBoolValue(const std::string& sectionName, const std::string& keyName) const;

        /**
         * @brief Checks if a specific section exists in the parsed INI data.
         * @param sectionName The name of the section to check.
         * @return True if the section exists, false otherwise.
         */
        bool HasSection(const std::string& sectionName) const;

        /**
         * @brief Checks if a specific key exists within a given section.
         * @param sectionName The name of the section.
         * @param keyName The name of the key to check.
         * @return True if the key exists in the specified section, false otherwise.
         */
        bool HasKey(const std::string& sectionName, const std::string& keyName) const;

        /**
         * @brief Dumps the parsed INI data to standard output for debugging.
         */
        void Dump() const;

    private:
        //! Stores the parsed INI data
        IniData m_data;
    };
}
