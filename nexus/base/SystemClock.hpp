//
//  SystemClock.hpp
//  Lapis
//
//  Created by nuttachai on 21/5/20.
//

#ifndef SystemClock_hpp
#define SystemClock_hpp

#include "nxsMacros.h"
#include <chrono>

NXS_NAMESPACE {
    /**
     * A utility class for high resolution timer.
     */
    class SystemClock
    {
    public:
        /// Set the marker.
        void tick();
        
        /// Get seconds since the previous tick.
        uint32_t getDiffSeconds() const;
        /// Get milliseconds since the previous tick.
        uint32_t getDiffMilliseconds() const;
        /// Get microseconds since the previous tick.
        uint64_t getDiffMicroseconds() const;
        
        /// Get time since the previous tick.
        template<typename _Duration>
        uint64_t getDiff() const
        {
            using namespace std::chrono;
            
            static_assert( __is_duration<_Duration>::value,
                        "Template parameter must be of type std::chrono::duration; for example, std::chrono::microseconds, std::chrono::milliseconds, etc." );
            
            auto now = time_point_cast<_Duration>( system_clock::now() );
            auto marker = time_point_cast<_Duration>( m_marker );
            auto diff = now - marker;
            return diff.count();
        }
        
        /// Get time since Epoch.
        template<typename _Duration>
        static uint64_t getNow()
        {
            using namespace std::chrono;

            static_assert( __is_duration<_Duration>::value,
                        "Template parameter must be of type std::chrono::duration; for example, std::chrono::microseconds, std::chrono::milliseconds, etc." );
            
            auto now = time_point_cast<_Duration>( system_clock::now() );
            auto value = now.time_since_epoch();
            return value.count();
        }
        
    private:
        std::chrono::time_point<std::chrono::system_clock> m_marker;
    };
}

#endif /* SystemClock_hpp */
