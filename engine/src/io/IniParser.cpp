//
// Created by nutta on 8/5/2025.
//
#include "nexus/io/IniParser.h"
#include "nexus/core/LogDispatcher.h"
#include "nexus/core/StringUtil.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(IniParser);

template<typename T>
requires std::floating_point
bool IsDouble(T value)
{
    return true;
}

bool IniParser::Load(const std::string& filePath)
{
    m_data.clear(); // Clear any previously loaded data

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG_ERROR(LogIniParser, std::format("Failed to open INI file: {}", filePath));
        return false;
    }

    std::string line;
    std::string currentSection = ""; // Default global section

    LOG_INFO(LogIniParser, std::format("Loading file: {}", filePath));

    while (std::getline(file, line))
    {
       line = StrUtil::Trim(line);

        if (line.empty() || line[0] == ';' || line[0] == '#')
        {
            // Skip empty lines and comment lines
            continue;
        }

        if (line[0] == '[' && line.back() == ']')
        {
            // It's a section header
            currentSection = StrUtil::Trim(line.substr(1, line.length() - 2));
            StrUtil::ToLower(currentSection); // Normalize for a map key
            m_data[currentSection]; // Ensure the section exists in the map
            LOG_DEBUG(LogIniParser, std::format("Found section: [{}]", currentSection));
        }
        else
        {
            // It's a key-value pair
            if (size_t eqPos = line.find('='); eqPos != std::string::npos)
            {
                std::string key = StrUtil::Trim(line.substr(0, eqPos));
                std::string value = StrUtil::Trim(line.substr(eqPos + 1));

                StrUtil::ToLower(key); // Normalize for a map key

                m_data[currentSection][key] = value;
                LOG_DEBUG(LogIniParser, std::format("Added key-value: [{}] {} = {}", currentSection, key, value));
            }
            else
            {
                LOG_WARNING(LogIniParser, std::format("Skipping malformed line (no '=' found): {}", line));
            }
        }
    }

    file.close();
    LOG_INFO(LogIniParser, std::format("Successfully loaded file: {}", filePath));
    return true;
}

std::optional<std::string> IniParser::GetValue(const std::string& sectionName, const std::string& keyName) const
{
    std::string normalizedSection = sectionName;
    StrUtil::ToLower(normalizedSection);

    std::string normalizedKey = keyName;
    StrUtil::ToLower(normalizedKey);

    if (const auto sectionIt = m_data.find(normalizedSection); sectionIt != m_data.end())
    {
        for (const auto& [key, value] : sectionIt->second)
        {
            LOG_DEBUG(LogIniParser, std::format("[{}]={}", key, value));
        }
        const auto keyIt = sectionIt->second.find(normalizedKey);
        if (keyIt != sectionIt->second.end())
        {
            return keyIt->second;
        }
    }

    LOG_WARNING(LogIniParser, std::format("Key '{}' not found in section '{}'.", keyName, sectionName));
    return std::nullopt;
}

std::optional<bool> IniParser::GetBoolValue(const std::string& sectionName, const std::string& keyName) const
{
    if (const auto value = GetValue(sectionName, keyName); value.has_value()) {
        return StrUtil::ParseBool(*value);
    }
    return std::nullopt;
}

bool IniParser::HasSection(const std::string& sectionName) const
{
    if (StrUtil::Trim(sectionName) == "") return false;

    std::string normalizedSection = sectionName;
    StrUtil::ToLower(normalizedSection);
    return m_data.contains(normalizedSection);
}

bool IniParser::HasKey(const std::string& sectionName, const std::string& keyName) const
{
    std::string normalizedSection = sectionName;
    StrUtil::ToLower(normalizedSection);

    std::string normalizedKey = keyName;
    StrUtil::ToLower(normalizedKey);

    if (const auto sectionIt = m_data.find(normalizedSection); sectionIt != m_data.end())
    {
        return sectionIt->second.contains(normalizedKey);
    }
    return false;
}

void IniParser::Dump() const
{
    LOG_INFO(LogIniParser, "--- INI Parser Dump ---");
    if (m_data.empty())
    {
        LOG_INFO(LogIniParser, "No data loaded.");
    }
    else
    {
        for (const auto& [fst, snd] : m_data)
        {
            if (!fst.empty())
            {
                LOG_INFO(LogIniParser, std::format("[{}]", fst));
            }
            else
            {
                LOG_INFO(LogIniParser, "[GLOBAL / DEFAULT]");
            }

            for (const auto& [fst, snd] : snd)
            {
                LOG_INFO(LogIniParser, std::format("  {}={}", fst, snd));
            }
        }
    }
    LOG_INFO(LogIniParser, "--- End INI Parser Dump ---");
}