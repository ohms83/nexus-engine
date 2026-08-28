//
// Created by nutta on 7/30/2025.
//

#include "nexus/time/StandardTimeSource.h"
#include <SDL3/SDL.h>

USING_NAMESPACE_NXS;

double StandardTimeSource::Now()
{
    return CAST<double>(SDL_GetTicks()) / 1000;
}

double StandardTimeSource::NowMs()
{
    return CAST<double>(SDL_GetTicks());
}
