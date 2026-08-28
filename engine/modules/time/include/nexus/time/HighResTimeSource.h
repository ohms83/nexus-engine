//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "TimeSource.h"

NXS_NAMESPACE
{
    //! High resolution time-provider. Usually used for profiling.
    class HighResTimeSource final : public ITimeSource
    {
    public:
        double Now() override;
        double NowMs() override;
    };
}
