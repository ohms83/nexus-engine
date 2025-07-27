//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    class Timer
    {
    public:
        Timer() = default;
        ~Timer() = default;

        //! Set the time stamp.
        void Stamp();

        NODISCARD float Seconds() const;
        //! Compute the delta time since the previous Stamp in seconds.
        NODISCARD float GetDeltaTime() const;
        
    private:
        //! A time stamp since the application started in milliseconds.
        uint64 m_tick = 0;
    };
}
