//
// Created by nutta on 7/30/2025.
//
#include "gtest/gtest.h"
#include "nexus/Nexus.h"

class FakeTimeSource final : public nxs::ITimeSource
{
public:
    double Now() override
    {
        return seconds_;
    }
    double NowMs() override
    {
        return seconds_ * 1000;
    }

    //! Manually advance to tick by the specified seconds
    void Advance(const double seconds)
    {
        seconds_ += seconds;
    }

private:
    double seconds_ = 0.0;
};

// Define a test fixture for MyTimer tests
class TimerTest : public ::testing::Test {
protected:
    // Shared pointers to manage lifetime and allow injection
    nxs::Ref<FakeTimeSource> time_source_;
    nxs::Ptr<nxs::Timer> timer_;

    // SetUp is called before each test
    void SetUp() override {
        time_source_ = std::make_shared<FakeTimeSource>();
        timer_ = std::make_unique<nxs::Timer>(time_source_);
    }

    // TearDown is called after each test (optional, for cleanup)
    void TearDown() override {
        // No explicit cleanup needed for smart pointers here
    }
};

TEST_F(TimerTest, ElapsedTime)
{
    std::array<float, 3> timeSteps = {
        5, 10, 20
    };
    float seconds = 0;

    timer_->Start();
    std::ranges::for_each(timeSteps, [&](const float dt)
    {
        seconds += dt;
        time_source_->Advance(dt);
        ASSERT_EQ(timer_->GetElapsedTime(), seconds);
    });
}

TEST_F(TimerTest, DeltaTime)
{
    std::array<float, 3> timeSteps = {
        5, 10, 20
    };

    timer_->Start();
    std::ranges::for_each(timeSteps, [&](const float dt)
    {
        time_source_->Advance(dt);
        timer_->Tick();
        ASSERT_EQ(timer_->GetDeltaTime(), dt);
    });
}

TEST_F(TimerTest, ScheduledAction)
{
    bool executed = false;
    timer_->Start();
    timer_->ScheduleAction([&] {
        executed = true;
        std::cout << "Scheduled action" << std::endl;
    }, 10);

    timer_->Tick();
    // Time is still 0. The action shouldn't be executed.
    ASSERT_FALSE(executed);

    // Advance the time just before the trigger point.
    time_source_->Advance(9);
    timer_->Tick();
    // The action shouldn't be executed yet.
    ASSERT_FALSE(executed);

    // Advance the time to the trigger point.
    time_source_->Advance(1);
    timer_->Tick();
    // The action should be executed.
    ASSERT_TRUE(executed);

    // Ensure it doesn't execute again on later updates (for one-shot tasks)
    executed = false;
    time_source_->Advance(10);
    timer_->Tick();
    ASSERT_FALSE(executed);
}

/**
 * Expected results:
 *   - Scheduled action must not be executed.
 *   - GetDeltaTime() and GetElapsedTime() must return 0.
 */
TEST_F(TimerTest, Stop)
{
    bool executed = false;
    timer_->Start();
    timer_->ScheduleAction([&] {
        executed = true;
    }, 50);

    time_source_->Advance(49);
    timer_->Tick();
    timer_->Stop();

    time_source_->Advance(1);
    timer_->Tick();
    ASSERT_FALSE(executed);
    ASSERT_EQ(timer_->GetDeltaTime(), 0);
    ASSERT_EQ(timer_->GetElapsedTime(), 0);
}