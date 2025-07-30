//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "TimeSource.h"

NXS_NAMESPACE
{
    //! Standard resolution time-provider
    class StandardTimeSource final : public ITimeSource
    {
    public:
        double Now() override;
        double NowMs() override;
    };
}
