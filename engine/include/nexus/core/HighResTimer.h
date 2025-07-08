//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    /**
     * A utility class for high resolution timer. This class is preferred for profiling.
     */
    class HighResTimer
    {
    public:
        HighResTimer() = default;
        ~HighResTimer() = default;

        //! Set the time stamp.
        void Stamp();

        //! Compute the delta time since the previous Stamp in seconds.
        float GetDeltaTime() const;

    private:
        //! The value of the high-resolution counter from the latest Stamp call.
        uint64 m_tick = 0;
    };
}
