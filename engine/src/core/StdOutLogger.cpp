//
// Created by nutta on 8/2/2025.
//
#include "nexus/core/StdOutLogger.h"
#include <iostream>

USING_NAMESPACE_NXS;

void StdOutLogger::Log(LogLevel level, const std::string& message)
{
    std::cout << message << std::endl;
}

void StdOutLogger::Flush()
{
    std::cout << std::flush;
}
