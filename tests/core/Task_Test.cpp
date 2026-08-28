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
        scheduler->TransferPendingTasks();
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

TEST(DelayTest, RemainsActiveUntilTheDelayDeadline) {
    auto mockTimeSource = std::make_shared<MockTimeSource>();
    mockTimeSource->SetTime(0.0);
    Delay delayTask(1.0, mockTimeSource);

    ASSERT_TRUE(delayTask.Update());

    mockTimeSource->SetTime(0.99);
    ASSERT_TRUE(delayTask.Update());

    mockTimeSource->SetTime(1.0);
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

TEST(RepeatTaskTest, ZeroRepeatTerminatesImmediately) {
    int counter = 0;
    RepeatTask repeatTask(0, [&]() {
        ++counter;
        return true;
    });

    ASSERT_FALSE(repeatTask.Update());
    ASSERT_EQ(counter, 0);
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

TEST(SequentialTaskTest, EmptySequenceTerminatesImmediately) {
    SequentialTask sequentialTask;

    ASSERT_FALSE(sequentialTask.Update());
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
TEST(IntervalTaskTest, ExecutesWhenIntervalHasElapsed) {
    auto mockTimeSource = std::make_shared<MockTimeSource>();
    mockTimeSource->SetTime(0.0);

    int callCount = 0;
    IntervalTask intervalTask(1.0, [&]() {
        ++callCount;
        return callCount < 3;
    }, mockTimeSource);

    ASSERT_TRUE(intervalTask.IsActive());
    ASSERT_TRUE(intervalTask.Update());
    EXPECT_EQ(callCount, 0);

    mockTimeSource->SetTime(0.5);
    ASSERT_TRUE(intervalTask.Update());
    EXPECT_EQ(callCount, 0);

    mockTimeSource->SetTime(1.0);
    ASSERT_TRUE(intervalTask.Update());
    EXPECT_EQ(callCount, 1);

    mockTimeSource->SetTime(1.5);
    ASSERT_TRUE(intervalTask.Update());
    EXPECT_EQ(callCount, 1);

    mockTimeSource->SetTime(2.0);
    ASSERT_TRUE(intervalTask.Update());
    EXPECT_EQ(callCount, 2);

    mockTimeSource->SetTime(3.0);
    ASSERT_FALSE(intervalTask.Update());
    ASSERT_FALSE(intervalTask.IsActive());
    EXPECT_EQ(callCount, 3);

    mockTimeSource->SetTime(4.0);
    ASSERT_FALSE(intervalTask.Update());
    ASSERT_FALSE(intervalTask.IsActive());
    EXPECT_EQ(callCount, 3);
}

// Test that the IntervalTask executes its action even if the interval is set to zero,
// meaning it should execute on every update call.
TEST(IntervalTaskTest, AlwaysExecutes) {
    auto mockTimeSource = std::make_shared<MockTimeSource>();
    mockTimeSource->SetTime(0.0);

    std::array<double, 2> testIntervals = { 0.0, -1.0 }; // Test both zero and negative intervals
    for (const auto interval : testIntervals)
    {
        int callCount = 0;
        IntervalTask intervalTask(interval, [&]() {
            ++callCount;
            return true; // Always returns true, so it should keep executing
        }, mockTimeSource);
        
        ASSERT_TRUE(intervalTask.Update());
        EXPECT_EQ(callCount, 1);

        ASSERT_TRUE(intervalTask.Update());
        EXPECT_EQ(callCount, 2);

        ASSERT_TRUE(intervalTask.Update());
        EXPECT_EQ(callCount, 3);
    }
}

TEST_F(TaskSchedulerTest, ScheduleAndStopTask) {
    bool taskExecuted = false;
    auto task = std::make_shared<OneshotTask>([&]() {
        taskExecuted = true;
    });

    // Schedule the task
    TaskID id = scheduler->ScheduleTask(task);
    scheduler->TransferPendingTasks();

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

    scheduler->TransferPendingTasks();

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

TEST(TaskGroupTest, ClearsAllTasks) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);
    ASSERT_EQ(group.GetNumTask(), 2);

    group.Clear();
    ASSERT_EQ(group.GetNumTask(), 0);
    ASSERT_TRUE(group.GetTasks().empty());
}

TEST(TaskGroupTest, RemovesTaskUsingPredicate) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();
    auto task3 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);
    group.Add(task3);

    // Remove task2 using predicate (e.g., based on some property, but since MockTask is simple, use pointer comparison)
    group.RemoveIf([&](Ref<IRunnable> task) { return task == task2; });

    ASSERT_EQ(group.GetNumTask(), 2);
    ASSERT_TRUE(std::find(group.GetTasks().begin(), group.GetTasks().end(), task2) == group.GetTasks().end());
    ASSERT_TRUE(std::find(group.GetTasks().begin(), group.GetTasks().end(), task1) != group.GetTasks().end());
    ASSERT_TRUE(std::find(group.GetTasks().begin(), group.GetTasks().end(), task3) != group.GetTasks().end());
}

TEST(TaskGroupTest, FindsTask) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);

    auto found = group.Find(task1);
    ASSERT_EQ(found, task1);

    auto notFound = group.Find(std::make_shared<MockTask>());
    ASSERT_EQ(notFound, nullptr);
}

TEST(TaskGroupTest, FindsTaskUsingPredicate) {
    TaskGroup group;
    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    group.Add(task2);

    // Find task1 using predicate
    auto found = group.FindIf([&](Ref<IRunnable> task) { return task == task1; });
    ASSERT_EQ(found, task1);

    // Find non-existent
    auto notFound = group.FindIf([&](Ref<IRunnable> task) { return false; });
    ASSERT_EQ(notFound, nullptr);
}

TEST(TaskGroupTest, MergesTaskGroups) {
    TaskGroup group1;
    TaskGroup group2;

    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();
    auto task3 = std::make_shared<MockTask>();
    auto task4 = std::make_shared<MockTask>();

    group1.Add(task1);
    group1.Add(task2);
    group2.Add(task3);
    group2.Add(task4);

    ASSERT_EQ(group1.GetNumTask(), 2);
    ASSERT_EQ(group2.GetNumTask(), 2);

    group1.Merge(group2);

    ASSERT_EQ(group1.GetNumTask(), 4);
    ASSERT_EQ(group2.GetNumTask(), 0);

    // Check that tasks are in group1
    auto tasks = group1.GetTasks();
    ASSERT_TRUE(std::find(tasks.begin(), tasks.end(), task1) != tasks.end());
    ASSERT_TRUE(std::find(tasks.begin(), tasks.end(), task2) != tasks.end());
    ASSERT_TRUE(std::find(tasks.begin(), tasks.end(), task3) != tasks.end());
    ASSERT_TRUE(std::find(tasks.begin(), tasks.end(), task4) != tasks.end());
}

TEST(TaskGroupTest, GetNumTaskReturnsCorrectCount) {
    TaskGroup group;
    ASSERT_EQ(group.GetNumTask(), 0);

    auto task1 = std::make_shared<MockTask>();
    auto task2 = std::make_shared<MockTask>();

    group.Add(task1);
    ASSERT_EQ(group.GetNumTask(), 1);

    group.Add(task2);
    ASSERT_EQ(group.GetNumTask(), 2);

    group.Remove(task1);
    ASSERT_EQ(group.GetNumTask(), 1);
}

// ============================================================================
// Additional Task Scheduler Tests for Updated Design
// ============================================================================

TEST_F(TaskSchedulerTest, GetAllTasksIncludesPending) {
    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});
    auto task3 = std::make_shared<OneshotTask>([]() {});

    // Schedule tasks but do not transfer yet
    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::PreUpdate);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);
    scheduler->ScheduleTask(task3, TaskScheduler::UpdatePhase::PostUpdate);

    // Get all tasks including pending
    auto allTasks = scheduler->GetAllTasks(true);
    ASSERT_EQ(allTasks.size(), 3);
    ASSERT_TRUE(std::find(allTasks.begin(), allTasks.end(), task1) != allTasks.end());
    ASSERT_TRUE(std::find(allTasks.begin(), allTasks.end(), task2) != allTasks.end());
    ASSERT_TRUE(std::find(allTasks.begin(), allTasks.end(), task3) != allTasks.end());

    // Transfer pending tasks
    scheduler->TransferPendingTasks();

    // Now get all tasks should still include them
    allTasks = scheduler->GetAllTasks(true);
    ASSERT_EQ(allTasks.size(), 3);
}

TEST_F(TaskSchedulerTest, GetAllTasksExcludesPending) {
    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});

    // Schedule tasks but do not transfer
    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::PreUpdate);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);

    // Get all tasks excluding pending (should be empty)
    auto allTasks = scheduler->GetAllTasks(false);
    ASSERT_EQ(allTasks.size(), 0);

    // Transfer pending tasks
    scheduler->TransferPendingTasks();

    // Now get all tasks excluding pending should include them
    allTasks = scheduler->GetAllTasks(false);
    ASSERT_EQ(allTasks.size(), 2);
    ASSERT_TRUE(std::find(allTasks.begin(), allTasks.end(), task1) != allTasks.end());
    ASSERT_TRUE(std::find(allTasks.begin(), allTasks.end(), task2) != allTasks.end());
}

TEST_F(TaskSchedulerTest, GetAllTasksFromGroup) {
    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});
    auto task3 = std::make_shared<OneshotTask>([]() {});

    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::Update);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);
    scheduler->ScheduleTask(task3, TaskScheduler::UpdatePhase::PostUpdate);
    scheduler->TransferPendingTasks();

    // Get tasks from Update phase
    auto updateTasks = scheduler->GetAllTasksFromGroup(TaskScheduler::UpdatePhase::Update, true);
    ASSERT_EQ(updateTasks.size(), 2);
    ASSERT_TRUE(std::find(updateTasks.begin(), updateTasks.end(), task1) != updateTasks.end());
    ASSERT_TRUE(std::find(updateTasks.begin(), updateTasks.end(), task2) != updateTasks.end());

    // Get tasks from PostUpdate phase
    auto postTasks = scheduler->GetAllTasksFromGroup(TaskScheduler::UpdatePhase::PostUpdate, true);
    ASSERT_EQ(postTasks.size(), 1);
    ASSERT_EQ(postTasks[0], task3);
}

TEST_F(TaskSchedulerTest, GetNumPending) {
    ASSERT_EQ(scheduler->GetNumPending(), 0);

    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});

    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::PreUpdate);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);

    ASSERT_EQ(scheduler->GetNumPending(), 2);

    scheduler->TransferPendingTasks();

    ASSERT_EQ(scheduler->GetNumPending(), 0);
}

TEST_F(TaskSchedulerTest, GetNumPendingFromGroup) {
    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});
    auto task3 = std::make_shared<OneshotTask>([]() {});

    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::PreUpdate);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);
    scheduler->ScheduleTask(task3, TaskScheduler::UpdatePhase::Update);

    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::PreUpdate), 1);
    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::Update), 2);
    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::PostUpdate), 0);

    scheduler->TransferPendingTasks();

    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::PreUpdate), 0);
    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::Update), 0);
    ASSERT_EQ(scheduler->GetNumPendingFromGroup(TaskScheduler::UpdatePhase::PostUpdate), 0);
}

TEST_F(TaskSchedulerTest, TransferPendingTasksMovesToGroups) {
    auto task1 = std::make_shared<OneshotTask>([]() {});
    auto task2 = std::make_shared<OneshotTask>([]() {});

    scheduler->ScheduleTask(task1, TaskScheduler::UpdatePhase::PreUpdate);
    scheduler->ScheduleTask(task2, TaskScheduler::UpdatePhase::Update);

    // Before transfer, groups should be empty
    ASSERT_EQ(scheduler->GetNumTaskInGroup(TaskScheduler::UpdatePhase::PreUpdate, false), 0);
    ASSERT_EQ(scheduler->GetNumTaskInGroup(TaskScheduler::UpdatePhase::Update, false), 0);

    scheduler->TransferPendingTasks();

    // After transfer, tasks should be in groups
    ASSERT_EQ(scheduler->GetNumTaskInGroup(TaskScheduler::UpdatePhase::PreUpdate, false), 1);
    ASSERT_EQ(scheduler->GetNumTaskInGroup(TaskScheduler::UpdatePhase::Update, false), 1);
}