#pragma once

#include "Runnable.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/Timer.h"

NXS_NAMESPACE
{
    class Delay final : public IRunnable
    {
    public:
        explicit Delay(double seconds, Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());
        virtual ~Delay() = default;

        MAYBE_UNUSED bool Update() override;

    private:
        Ptr<Timer> m_timer;
        bool m_timeout = false;
    };
}