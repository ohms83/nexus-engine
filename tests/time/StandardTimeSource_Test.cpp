//
// Created by nutta on 7/31/2025.
//
#include "gtest/gtest.h"
#include "nexus/Nexus.h"

#include <thread>               // For std::this_thread::sleep_for
#include <chrono>               // For std::chrono::milliseconds, seconds

USING_NAMESPACE_NXS;

// Define a test fixture for StandardTimeSource
class StandardTimeSourceTest : public ::testing::Test {
protected:
    Ptr<StandardTimeSource> timeSource;

    void SetUp() override {
        // Initialize SDL with minimal necessary subsystems for headless operation
        if (!SDL_Init(SDL_INIT_EVENTS)) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        std::cout << "SDL initialized headless.\n";

        timeSource.reset(new StandardTimeSource());
    }

    void TearDown() override {
        timeSource.reset();
        SDL_Quit();
    }
};

// --- Test Cases for StandardTimeSource ---

// Test 1: Monotonicity - Now() should always return non-decreasing values
// This is a fundamental property of a reliable time source.
TEST_F(StandardTimeSourceTest, NowIsMonotonic) {
    double t1 = timeSource->Now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Small delay
    double t2 = timeSource->Now();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Longer delay
    double t3 = timeSource->Now();

    // Time should always be greater than or equal to the previous measurement.
    EXPECT_GE(t2, t1);
    EXPECT_GE(t3, t2);
}

// Test 2: Monotonicity - NowMs() should always return non-decreasing values
TEST_F(StandardTimeSourceTest, NowMsIsMonotonic) {
    double t1_ms = timeSource->NowMs();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    double t2_ms = timeSource->NowMs();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double t3_ms = timeSource->NowMs();

    EXPECT_GE(t2_ms, t1_ms);
    EXPECT_GE(t3_ms, t2_ms);
}

// Test 3: Consistency between Now() and NowMs()
// NowMs() should be 1000 * Now(), within floating-point tolerance.
TEST_F(StandardTimeSourceTest, NowAndNowMsAreConsistentWithinTolerance) {
    double now_s = timeSource->Now();
    double now_ms = timeSource->NowMs();

    // Use a small epsilon for floating-point comparisons.
    // 1e-6 seconds (1 microsecond) is a common tolerance. Adjust if needed based on
    // your system's timer precision and expected behavior.
    const double EPSILON_S = 1e-6;   // Tolerance in seconds
    const double EPSILON_MS = 1e-3;  // Tolerance in milliseconds (1 microsecond * 1000)

    EXPECT_NEAR(now_ms, now_s * 1000.0, EPSILON_MS);
    EXPECT_NEAR(now_s, now_ms / 1000.0, EPSILON_S);

    // Advance time and check consistency again
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    now_s = timeSource->Now();
    now_ms = timeSource->NowMs();
    EXPECT_NEAR(now_ms, now_s * 1000.0, EPSILON_MS);
}

// Test 4: Relative Time Accuracy (for a known sleep duration)
// This verifies that measuring a sleep period yields a value close to the expected duration.
TEST_F(StandardTimeSourceTest, MeasureSleepDurationAccuracy) {
    constexpr double SLEEP_DURATION_MS = 50.0; // Expected sleep in milliseconds
    constexpr double SLEEP_DURATION_S = SLEEP_DURATION_MS / 1000.0; // Expected sleep in seconds

    double start_s = timeSource->Now();
    double start_ms = timeSource->NowMs();

    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(SLEEP_DURATION_MS)));

    double end_s = timeSource->Now();
    double end_ms = timeSource->NowMs();

    double delta_s = end_s - start_s;
    double delta_ms = end_ms - start_ms;

    // IMPORTANT: Operating system scheduling is not perfectly precise.
    // You'll need a higher tolerance here compared to internal consistency checks.
    // 5-10ms tolerance is often acceptable for common OSes, but this can vary.
    constexpr double EPSILON_MEASUREMENT_S = 0.010; // e.g., allow up to 10 milliseconds deviation
    constexpr double EPSILON_MEASUREMENT_MS = EPSILON_MEASUREMENT_S * 1000.0;

    EXPECT_NEAR(delta_s, SLEEP_DURATION_S, EPSILON_MEASUREMENT_S);
    EXPECT_NEAR(delta_ms, SLEEP_DURATION_MS, EPSILON_MEASUREMENT_MS);
}

// Test 5: Handle very short delays (to probe timer resolution limits)
// This tests how the timer behaves with minimal time advances.
TEST_F(StandardTimeSourceTest, HandlesVeryShortDelays) {
    double start_s = timeSource->Now();
    std::this_thread::sleep_for(std::chrono::microseconds(1)); // Sleep for 1 microsecond
    double end_s = timeSource->Now();
    double delta_s = end_s - start_s;

    // The delta should be non-negative. It might be 0 if the system's timer resolution
    // is coarser than the requested sleep duration.
    EXPECT_GE(delta_s, 0.0);
    // It should also be a small positive number, e.g., less than 10ms.
    EXPECT_LT(delta_s, 0.010);
}

// Test 6: Long-term Monotonicity - Ensure no backward jumps after a significant period.
TEST_F(StandardTimeSourceTest, LongTermMonotonicity) {
    double t_start = timeSource->Now();
    // Sleep for a noticeable amount, e.g., 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    double t_after_sleep = timeSource->Now();

    // After a real time delay, the time must not go backward.
    EXPECT_GE(t_after_sleep, t_start);
}