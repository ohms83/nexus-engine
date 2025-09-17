#pragma once

#include "Runnable.h"

#include "nexus/time/TimeSource.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/Timer.h"

NXS_NAMESPACE
{
    class DelayTask final : public IRunnable
    {
    public:
        explicit DelayTask(double seconds, Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());
        virtual ~DelayTask() = default;

        bool Update() override;

    private:
        Ptr<Timer> m_timer;
        bool m_timeout = false;
    };
}