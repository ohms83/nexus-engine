#pragma once

#include "Runnable.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/Timer.h"

NXS_NAMESPACE
{
    class IntervalTask final : public IRunnable
    {
    public:
        IntervalTask(double interval, TaskFunc task, Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());
        ~IntervalTask() override = default;

        MAYBE_UNUSED bool Update() override;
    private:
        Ptr<Timer> m_timer;
        TaskFunc m_task;
        double m_interval = 0;
        bool m_deferred = false;
    };
}