//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <chrono>
#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    template<typename T>
    concept is_duration = requires {
        typename T::rep;
        typename T::period;
    } && std::is_base_of_v<std::chrono::duration<typename T::rep, typename T::period>, T>;

    /**
     * A utility class for high resolution timer.
     */
    class SystemClock
    {
    public:
        SystemClock() = default;
        ~SystemClock() = default;

        /// Set the marker.
        void Tick();
        
        /// Get seconds since the previous tick.
        uint32 GetDiffSeconds() const;
        /// Get milliseconds since the previous tick.
        uint32 GetDiffMilliseconds() const;
        /// Get microseconds since the previous tick.
        uint64 GetDiffMicroseconds() const;
        
        /// Get time since the previous tick.
        template<typename _Duration>
        uint64 GetDiff() const
        {
            using namespace std::chrono;
            
            auto now = time_point_cast<_Duration>( system_clock::now() );
            auto marker = time_point_cast<_Duration>( m_marker );
            auto diff = now - marker;
            return diff.count();
        }
        
        /// Get time since Epoch.
        template<typename _Duration>
        requires is_duration<_Duration>
        static uint64 GetNow()
        {
            using namespace std::chrono;
            auto now = time_point_cast<_Duration>( system_clock::now() );
            auto value = now.time_since_epoch();
            return value.count();
        }
        
    private:
        std::chrono::time_point<std::chrono::system_clock> m_marker;
    };
}
