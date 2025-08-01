//
// Created by nutta on 7/30/2025.
//

#include "nexus/time/HighResTimeSource.h"

USING_NAMESPACE_NXS;

double HighResTimeSource::Now()
{
    return CAST<double>(SDL_GetPerformanceCounter()) / CAST<double>(SDL_GetPerformanceFrequency());
}

double HighResTimeSource::NowMs()
{
    return CAST<double>(SDL_GetPerformanceCounter()) / CAST<double>(SDL_GetPerformanceFrequency()) * 1000;
}
