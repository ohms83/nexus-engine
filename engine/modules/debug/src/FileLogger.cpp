//
// Created by nutta on 8/2/2025.
//
#include "nexus/debug/FileLogger.h"

#include <format>
#include <iostream>

using namespace nxs;

FileLogger::FileLogger(const std::string& fileName)
{
    m_logName = fileName;
    m_logPath = std::format("{}/logs/{}", NXS_PROJECT_PATH, m_logName);

    std::cout << "Opening log file: " << m_logPath << std::endl;
    m_logFile.open(m_logPath, std::ios::out);
}

FileLogger::~FileLogger()
{
    m_logFile.close();
}

void FileLogger::Log(LogLevel level, const std::string& message)
{
    m_logFile << message;
}

void FileLogger::Flush()
{
    m_logFile.flush();
}