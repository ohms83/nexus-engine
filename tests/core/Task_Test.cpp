#include <gtest/gtest.h>
#include "nexus/Nexus.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <functional>
#include <vector>
#include <map>
#include <ranges>
#include <future>
#include <stdexcept>
#include <cstdint>

USING_NAMESPACE_NXS;

class MockTimeSource : public ITimeSource {
public:
    double Now() override { return m_currentTime; }
    double NowMs() override { return Now() * 1000; }
    void SetTime(double time) { m_currentTime = time; }
private:
    double m_currentTime = 0.0;
};

// Test fixture for TaskScheduler
class TaskSchedulerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockTimeSource = std::make_shared<MockTimeSource>();
        scheduler = std::make_unique<TaskScheduler>(mockTimeSource);
    }

    void ScheduleTasks(nxs::TaskScheduler::UpdatePhase phase, size_t num)
    {
        std::vector<TaskID>& ids = taskIds[phase];
        for (size_t i = 0; i < num; ++i) {
            ids.push_back(scheduler->ScheduleTask(std::make_shared<OneshotTask>([&]() {}), phase));
        }
    }

    void StopTasks(nxs::TaskScheduler::UpdatePhase phase)
    {
        std::vector<TaskID>& ids = taskIds[phase];
        for (const auto id : ids) {
            scheduler->StopTask(id);
        }
        ids.clear();
    }

    Ref<MockTimeSource> mockTimeSource;
    std::unique_ptr<TaskScheduler> scheduler;
    std::map<nxs::TaskScheduler::UpdatePhase, std::vector<TaskID>> taskIds;
};

// ============================================================================
// Delay Task
// ============================================================================
TEST(DelayTest, FinishesAfterSpecifiedTime) {
    auto mockTimeSource = std::make_shared<MockTimeSource>();
    mockTimeSource->SetTime(0.0);
    Delay delayTask(2.5, mockTimeSource);

    // Initial state: task should be active
    ASSERT_TRUE(delayTask.Update());

    // Update time to less than the delay
    mockTimeSource->SetTime(1.0);
    ASSERT_TRUE(delayTask.Update());

    // Update time to exactly the delay
    mockTimeSource->SetTime(2.5);
    ASSERT_FALSE(delayTask.Update());

    // Update time to after the delay
    mockTimeSource->SetTime(2.6);
    ASSERT_FALSE(delayTask.Update());

    // Subsequent updates should also return false
    ASSERT_FALSE(delayTask.Update());
}

// ============================================================================
// Repeat Task
// ============================================================================
TEST(RepeatTaskTest, RepeatsCorrectNumberOfTimes) {
    int counter = 0;
    auto taskFunc = [&]() {
        counter++;
        return true;
    };
    RepeatTask repeatTask(3, taskFunc);

    // Run the task 3 times
    ASSERT_TRUE(repeatTask.Update()); // 1
    ASSERT_TRUE(repeatTask.Update()); // 2
    ASSERT_TRUE(repeatTask.Update()); // 3
    ASSERT_EQ(counter, 3);

    // The fourth call should finish the task
    ASSERT_FALSE(repeatTask.Update());
    ASSERT_EQ(counter, 3);
}

TEST(RepeatTaskTest, IndefiniteRepeatStopsWhenTaskReturnsFalse) {
    int counter = 0;
    auto taskFunc = [&]() {
        counter++;
        return counter < 5; // Task returns false after 4 runs
    };
    RepeatTask repeatTask(-1, taskFunc); // -1 for indefinite repeat

    // Run 4 times
    ASSERT_TRUE(repeatTask.Update()); // 1
    ASSERT_TRUE(repeatTask.Update()); // 2
    ASSERT_TRUE(repeatTask.Update()); // 3
    ASSERT_TRUE(repeatTask.Update()); // 4
    ASSERT_EQ(counter, 4);

    // The fifth call should return false
    ASSERT_FALSE(repeatTask.Update());
    ASSERT_EQ(counter, 5);
}

// ============================================================================
// Sequential Task
// ============================================================================
TEST(SequentialTaskTest, ExecutesTasksInOrder) {
    int counter = 0;
    auto task1 = std::make_shared<OneshotTask>([&]() { counter = 1; });
    auto task2 = std::make_shared<OneshotTask>([&]() { counter = 2; });

    SequentialTask sequentialTask;
    sequentialTask.Push(task1);
    sequentialTask.Push(task2);

    // Initial state: first task should run and finish
    ASSERT_TRUE(sequentialTask.Update());
    ASSERT_EQ(counter, 1);

    // Next update: second task should run and finish
    ASSERT_TRUE(sequentialTask.Update());
    ASSERT_EQ(counter, 2);

    // Third update: no more tasks, so the sequential task finishes
    ASSERT_FALSE(sequentialTask.Update());
    ASSERT_EQ(counter, 2);
}

TEST(SequentialTaskTest, InitializerList) {
    int counter = 0;
    auto task1 = std::make_shared<OneshotTask>([&]() { counter = 1; });
    auto task2 = std::make_shared<OneshotTask>([&]() { counter = 2; });

    SequentialTask sequentialTask = { task1, task2 };

    // Initial state: first task should run and finish
    ASSERT_TRUE(sequentialTask.Update());
    ASSERT_EQ(counter, 1);

    // Next update: second task should run and finish
    ASSERT_TRUE(sequentialTask.Update());
    ASSERT_EQ(counter, 2);

    // Third update: no more tasks, so the sequential task finishes
    ASSERT_FALSE(sequentialTask.Update());
    ASSERT_EQ(counter, 2);
}

// ============================================================================
// Task Scheduler
// ============================================================================
TEST_F(TaskSchedulerTest, ScheduleAndStopTask) {
    bool taskExecuted = false;
    auto task = std::make_shared<OneshotTask>([&]() {
        taskExecuted = true;
    });

    // Schedule the task
    TaskID id = scheduler->ScheduleTask(task);
    ASSERT_EQ(id, 1);

    // Update the scheduler, task should run
    scheduler->Update();
    ASSERT_TRUE(taskExecuted);

    // Reset the flag and stop the task
    taskExecuted = false;
    scheduler->StopTask(id);

    // Update again, task should not run
    scheduler->Update();
    ASSERT_FALSE(taskExecuted);
}

TEST_F(TaskSchedulerTest, ScheduleToCorrectPhases) {
    bool preUpdateRan = false;
    bool updateRan = false;
    bool postUpdateRan = false;

    scheduler->ScheduleTask(std::make_shared<OneshotTask>([&]() {
        preUpdateRan = true;
    }), TaskScheduler::UpdatePhase::PreUpdate);

    scheduler->ScheduleTask(std::make_shared<OneshotTask>([&]() {
        updateRan = true;
    }), TaskScheduler::UpdatePhase::Update);

    scheduler->ScheduleTask(std::make_shared<OneshotTask>([&]() {
        postUpdateRan = true;
    }), TaskScheduler::UpdatePhase::PostUpdate);

    // Run PreUpdate phase, only preUpdateRan should be true
    scheduler->PreUpdate();
    ASSERT_TRUE(preUpdateRan);
    ASSERT_FALSE(updateRan);
    ASSERT_FALSE(postUpdateRan);

    // Run Update phase, now updateRan should also be true
    scheduler->Update();
    ASSERT_TRUE(preUpdateRan);
    ASSERT_TRUE(updateRan);
    ASSERT_FALSE(postUpdateRan);

    // Run PostUpdate phase, all should be true
    scheduler->PostUpdate();
    ASSERT_TRUE(preUpdateRan);
    ASSERT_TRUE(updateRan);
    ASSERT_TRUE(postUpdateRan);
}

// Test whether the scheduled tasks are added and removed up correctly.
TEST_F(TaskSchedulerTest, HandleTaskTermination) {
    ASSERT_EQ(scheduler->GetNumTask(), 0);

    // PreUpdate
    {
        const TaskScheduler::UpdatePhase phase = TaskScheduler::UpdatePhase::PreUpdate;
        const size_t numTask = 1;

        ScheduleTasks(phase, numTask);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), numTask);
        ASSERT_EQ(scheduler->GetNumTask(), numTask);

        StopTasks(phase);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
        
        // Schedules the tasks again and lets it stop by its own to check whether the scheduler
        // will handle task termination gracefully.
        ScheduleTasks(phase, numTask);
        scheduler->PreUpdate();

        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
    }
    // Update
    {
        const TaskScheduler::UpdatePhase phase = TaskScheduler::UpdatePhase::Update;
        const size_t numTask = 2;

        ScheduleTasks(phase, numTask);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), numTask);
        ASSERT_EQ(scheduler->GetNumTask(), numTask);

        StopTasks(phase);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
        
        // Schedules the tasks again and lets it stop by its own to check whether the scheduler
        // will handle task termination gracefully.
        ScheduleTasks(phase, numTask);
        scheduler->Update();

        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
    }
    // PostUpdate
    {
        const TaskScheduler::UpdatePhase phase = TaskScheduler::UpdatePhase::PostUpdate;
        const size_t numTask = 3;

        ScheduleTasks(phase, numTask);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), numTask);
        ASSERT_EQ(scheduler->GetNumTask(), numTask);

        StopTasks(phase);
        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
        
        // Schedules the tasks again and lets it stop by its own to check whether the scheduler
        // will handle task termination gracefully.
        ScheduleTasks(phase, numTask);
        scheduler->PostUpdate();

        ASSERT_EQ(scheduler->GetNumTaskInGroup(phase), 0);
        ASSERT_EQ(scheduler->GetNumTask(), 0);
    }
}

// Simple runnable task for testing
class MockTask : public IRunnable {
public:
    MockTask(bool shouldTerminate = false) : m_shouldTerminate(shouldTerminate) {}
    bool Update() override {
        m_updateCount++;
        return !m_shouldTerminate;
    }
    int GetUpdateCount() const { return m_updateCount; }
private:
    int m_updateCount = 0;
    bool m_shouldTerminate;
};

// ============================================================================
// Task Group Test Suite
// ============================================================================
TEST(TaskGroupTest, AddsAndUpdatesMultipleTasks) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);

    ASSERT_TRUE(group.Update());
    ASSERT_EQ(task1->GetUpdateCount(), 1);
    ASSERT_EQ(task2->GetUpdateCount(), 1);

    ASSERT_TRUE(group.Update());
    ASSERT_EQ(task1->GetUpdateCount(), 2);
    ASSERT_EQ(task2->GetUpdateCount(), 2);
}

TEST(TaskGroupTest, RemovesTaskFromGroup) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);

    group.Update();
    ASSERT_EQ(task1->GetUpdateCount(), 1);

    group.Remove(task1);
    group.Update();
    ASSERT_EQ(task1->GetUpdateCount(), 1); // Should not have been updated again
    ASSERT_EQ(task2->GetUpdateCount(), 2); // Other task still running
    ASSERT_TRUE(group.Update()); // Still active because task2 is running
}

TEST(TaskGroupTest, AutoRemovesTerminatedTasks) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>(true); // This task will terminate
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);

    ASSERT_TRUE(group.Update()); // task1 finishes, task2 runs once
    ASSERT_EQ(task1->GetUpdateCount(), 1);
    ASSERT_EQ(task2->GetUpdateCount(), 1);

    // After the first update, task1 should be removed
    ASSERT_TRUE(group.Update()); // Only task2 should update
    ASSERT_EQ(task1->GetUpdateCount(), 1);
    ASSERT_EQ(task2->GetUpdateCount(), 2);
}

TEST(TaskGroupTest, SignalsWhenTaskTerminates) {
    TaskGroup group;
    auto terminatedTask = std::make_shared<MockTask>(true);
    auto ongoingTask = std::make_shared<MockTask>();
    Ref<IRunnable> receivedTask = nullptr;

    group.taskFinishedCallback.connect([&](Ref<IRunnable> task) {
        receivedTask = task;
    });

    group.Add(terminatedTask);
    group.Add(ongoingTask);

    ASSERT_TRUE(group.Update());
    ASSERT_EQ(receivedTask, terminatedTask); // Callback should receive the terminated task

    receivedTask.reset();
    ASSERT_TRUE(group.Update());
    ASSERT_NE(receivedTask, terminatedTask); // Callback should not have been called again
}

TEST(TaskGroupTest, FinishesWhenAllTasksTerminate) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>(true);
    auto task2 = std::make_shared<MockTask>(true);

    group.Add(task1);
    group.Add(task2);

    // First update: both tasks should run and terminate
    ASSERT_FALSE(group.Update());
    ASSERT_EQ(task1->GetUpdateCount(), 1);
    ASSERT_EQ(task2->GetUpdateCount(), 1);
}