//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <functional>
#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    using TimerCallback = std::function<void()>;

    class ITimer
    {
    public:
        virtual ~ITimer() = default;

        /**
         * Advance the timer counter and fire off the registerd timer callback.
         * @return @c true, if the countdown event was fired, otherwise, @c false.
         */
        bool Tick();

        uint64 GetTick() const
        {
            return m_tick;
        }

        uint64 GetPrevTick() const
        {
            return m_prevTick;
        }

        //! Compute the delta time since the previous @c Tick in seconds.
        NODISCARD virtual float GetDeltaTime() const = 0;

        void CountDown(float seconds, const TimerCallback& callback);

    protected:
        virtual uint64 Tick_Internal() = 0;

    private:
        /**
         * A timer counter. This can be interpreted differently for each timer class.
         * Please be careful and avoid using it as-is; unless you know what you are doing.
         */
        uint64 m_tick = 0;
        //! A time stamp from the previous @c Tick.
        uint64 m_prevTick = 0;
        TimerCallback m_callback;
        float m_countDown = 0.0f;
    };

    class Timer final : public ITimer
    {
    public:
        ~Timer() override = default;

        //! Compute the delta time since the previous @c Tick in seconds.
        NODISCARD float GetDeltaTime() const override;

    protected:
        uint64 Tick_Internal() override;
    };
}
