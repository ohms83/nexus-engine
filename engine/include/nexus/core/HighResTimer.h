//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "Timer.h"

NXS_NAMESPACE
{
    /**
     * A utility class for high resolution timer. This class is preferred for profiling.
     */
    class HighResTimer final : public ITimer
    {
    public:
        ~HighResTimer() override = default;

        //! Compute the delta time since the previous Stamp in seconds.
        float GetDeltaTime() const override;

    protected:
        //! Advance the timer counter
        uint64 Tick_Internal() override;

    private:
        //! The value of the high-resolution counter from the latest Stamp call.
        uint64 m_tick = 0;
    };
}
