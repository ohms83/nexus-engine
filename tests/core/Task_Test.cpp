
#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "../helpers/FakeTimeSource.h"

DEFINE_LOG(TaskTest);

// Define a test fixture for SDL-dependent tests
class TaskTest : public ::testing::Test {
protected:
    // SetUp is called before each test in this fixture
    void SetUp() override {
        nxs::Logger::Init(nxs::Logger::LogToFile | nxs::Logger::LogToStdOut);
    }

    // TearDown is called after each test in this fixture
    void TearDown() override {
        nxs::Logger::Destroy();
    }
};

// Test whether the task runs correctly at the specified interval
TEST_F(TaskTest, RunTask)
{
    int numExecution = 0;
    auto timeSource = std::make_shared<FakeTimeSource>();
    auto task = std::make_unique<nxs::Task> (
        [&]() { ++numExecution; },
        // Repeat counts
        -1,
        // Delay
        0,
        // Interval
        10,
        // Run immediately
        true,
        // Time source
        timeSource
    );
    task->Update();
    // The task should be fired off immediately with runImmediate flag.
    EXPECT_EQ(numExecution, 1);

    timeSource->Advance(9.9);
    task->Update();
    // The task shouldn't have been repeated yet.
    EXPECT_EQ(numExecution, 1);
    
    timeSource->Advance(0.101);
    task->Update();
    // The task should be executed for the second time.
    EXPECT_EQ(numExecution, 2);
    
    timeSource->Advance(10.01);
    task->Update();
    // The task should be executed for the second time.
    EXPECT_EQ(numExecution, 3);
}


// Test whether the task runs at every Update when the interval is set to 0.
TEST_F(TaskTest, RunEveryFrame)
{
    int numExecution = 0;
    auto task = std::make_unique<nxs::Task> (
        [&]() { ++numExecution; },
        // Repeat counts
        -1,
        // Delay
        0,
        // Interval
        0,
        // Run immediately
        true
    );
    task->Update();
    EXPECT_EQ(numExecution, 1);
    task->Update();
    EXPECT_EQ(numExecution, 2);
    task->Update();
    EXPECT_EQ(numExecution, 3);
}

// Test whether the task starts after the specified delay.
TEST_F(TaskTest, DelayedTask)
{
    int numExecution = 0;
    auto timeSource = std::make_shared<FakeTimeSource>();
    auto task = std::make_unique<nxs::Task> (
        [&]() { ++numExecution; },
        // Repeat counts
        1,
        // Delay
        5,
        // Interval
        0,
        // Run immediately
        true,
        // Time source
        timeSource
    );
    task->Update();
    // The task shouldn't have been executed yet.
    EXPECT_EQ(numExecution, 0);

    timeSource->Advance(4.9);
    task->Update();
    // The task shouldn't have been executed yet.
    EXPECT_EQ(numExecution, 0);

    timeSource->Advance(1.01);
    task->Update();
    // The task should have already been executed.
    EXPECT_EQ(numExecution, 1);

    timeSource->Advance(5.01);
    task->Update();
    // The task shouldn't have been executed anymore.
    EXPECT_EQ(numExecution, 1);
}


// Test whether the task runs correctly at the specified interval
TEST_F(TaskTest, RepeatedTask)
{
    int numExecution = 0;
    constexpr int numRepeat = 10;
    auto task = std::make_unique<nxs::Task> (
        [&]() { ++numExecution; },
        // Repeat counts
        10,
        // Delay
        0,
        // Interval
        0,
        // Run immediately
        true
    );

    for (int i = 0; i < numRepeat; ++i)
    {
        task->Update();
        const int expectedResult = i + 1;
        EXPECT_EQ(numExecution, expectedResult);
    }
    
    // The task shouldn't be triggered anymore.
    task->Update();
    EXPECT_EQ(numExecution, numRepeat);
}