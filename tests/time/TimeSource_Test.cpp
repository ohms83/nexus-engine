//
// Created by nutta on 7/30/2025.
//
#include "gtest/gtest.h"
#include "nexus/Nexus.h"

// Define a test fixture for SDL-dependent tests
class TimeSourceTest : public ::testing::Test {
protected:
    // SetUp is called before each test in this fixture
    void SetUp() override {
        // Initialize SDL with minimal necessary subsystems for headless operation
        if (!SDL_Init(SDL_INIT_EVENTS)) {
            FAIL() << "SDL_Init failed: " << SDL_GetError();
        }
        std::cout << "SDL initialized headless.\n";
    }

    // TearDown is called after each test in this fixture
    void TearDown() override {
        SDL_Quit(); // Always call SDL_Quit() to clean up initialized subsystems
        std::cout << "SDL quit.\n";
    }
};

TEST_F(TimeSourceTest, StandardNow)
{
    const nxs::Ptr<nxs::StandardTimeSource> timeSource;
    auto diff = timeSource->Now();
    SDL_Delay(500);
    diff = timeSource->Now() - diff;
    std::cout << std::format("Diff={}\n", diff);
    EXPECT_NEAR(diff, 0.5, 0.0011);
}

TEST_F(TimeSourceTest, StandardNowMs)
{
    const nxs::Ptr<nxs::StandardTimeSource> timeSource;
    auto diffMs = timeSource->NowMs();
    SDL_Delay(500);
    diffMs = timeSource->NowMs() - diffMs;
    std::cout << std::format("Diff(ms)={}\n", diffMs);
    EXPECT_NEAR(diffMs, 500, 0.0005);
}

TEST_F(TimeSourceTest, HighResNow)
{
    const nxs::Ptr<nxs::HighResTimeSource> timeSource;
    auto diff = timeSource->Now();
    SDL_Delay(500);
    diff = timeSource->Now() - diff;
    std::cout << std::format("Diff={}\n", diff);
    EXPECT_NEAR(diff, 0.5, 0.0005);
}

TEST_F(TimeSourceTest, HighResNowMs)
{
    const nxs::Ptr<nxs::StandardTimeSource> timeSource;
    auto diffMs = timeSource->NowMs();
    SDL_Delay(500);
    diffMs = timeSource->NowMs() - diffMs;
    std::cout << std::format("Diff(ms)={}\n", diffMs);
    EXPECT_NEAR(diffMs, 500, 0.0005);
}