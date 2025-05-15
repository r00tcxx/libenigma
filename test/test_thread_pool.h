#pragma once
#include "gtest/gtest.h"
#include "thread_pool/thread_pool.h"

using namespace ema;

class TestThreadPool : public testing::Test {
protected:
    static void SetUpTestSuite() {}
    static void TearDownTestSuite() {}

    void SetUp() override {
        pool = std::make_shared<thread_pool>();
    }

    void TearDown() override {
        if (pool) {
            pool->shutdown();
        }
    }

    std::shared_ptr<thread_pool> pool;
};

// Test basic startup and shutdown
TEST_F(TestThreadPool, StartupShutdown) {
    // Start with 4 threads and 100 max tasks
    pool->startup(4, 100);
    
    // Shutdown should not cause any issues
    pool->shutdown();
    
    // Should be able to start up again after shutdown
    pool->startup(2, 50);
    pool->shutdown();
}

// Test startup with zero threads (should still create at least one)
TEST_F(TestThreadPool, StartupWithZeroThreads) {
    pool->startup(0, 100);
    
    // Submit a task to verify the pool is working
    std::atomic<bool> task_completed = false;
    pool->submit([&task_completed]() {
        task_completed = true;
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(task_completed);
    
    pool->shutdown();
}

// Test startup with disabling expansion
TEST_F(TestThreadPool, DisableExpansion) {
    // Start with 2 threads, 20 max tasks, no expansion allowed
    pool->startup(2, 20, false);
    
    std::atomic<int> counter = 0;
    std::vector<bool> results;
    
    // Submit tasks up to and beyond max task count
    for (int i = 0; i < 30; i++) {
		bool result = pool->submit([&counter]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
            counter++;
        });
        results.push_back(result);
    }
    
    // All submissions should succeed until max_task_count is reached
    for (size_t i = 0; i < 20 && i < results.size(); i++) {
        EXPECT_TRUE(results[i]) << "Task " << i << " should have been accepted";
    }
    
    // Submissions beyond max_task_count should fail
    for (size_t i = 20; i < results.size(); i++) {
        EXPECT_FALSE(results[i]) << "Task " << i << " should have been rejected";
    }
    
    // Wait for tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Should have processed only the accepted tasks
    EXPECT_EQ(counter, 20);
    
    pool->shutdown();
}

// Test submitting tasks to the thread pool
TEST_F(TestThreadPool, SubmitTasks) {
    pool->startup(4, 100);
    
    std::atomic<int> counter = 0;
    std::vector<bool> tasks_completed(10, false);
    
    for (int i = 0; i < 10; i++) {
        pool->submit([i, &counter, &tasks_completed]() {
            counter++;
            tasks_completed[i] = true;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(counter, 10);
    
    // Check that all tasks were completed
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(tasks_completed[i]);
    }
    
    pool->shutdown();
}

// Test submitting tasks with arguments
TEST_F(TestThreadPool, SubmitTasksWithArguments) {
    pool->startup(4, 100);
    
    std::atomic<int> sum = 0;
    
    auto add_to_sum = [&sum](int value) {
        sum += value;
    };
    
    for (int i = 1; i <= 10; i++) {
        pool->submit(add_to_sum, i);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(sum, 55); // Sum of numbers 1 to 10
    
    pool->shutdown();
}

// Test exec with return value
TEST_F(TestThreadPool, ExecWithReturnValue) {
    pool->startup(4, 100);
    
    auto future = pool->exec([](int a, int b) {
        return a + b;
    }, 5, 10);
    
    EXPECT_EQ(future.get(), 15);
    
    pool->shutdown();
}

// Test exec with void return type
TEST_F(TestThreadPool, ExecWithVoidReturn) {
    pool->startup(4, 100);
    
    std::atomic<bool> task_executed = false;
    
    auto future = pool->exec([&task_executed]() {
        task_executed = true;
    });
    
    future.get(); // Wait for completion
    EXPECT_TRUE(task_executed);
    
    pool->shutdown();
}

// Test exec with exception
TEST_F(TestThreadPool, ExecWithException) {
    pool->startup(4, 100);
    
    auto future = pool->exec([]() {
        throw std::runtime_error("Test exception");
        return 42;
    });
    
    EXPECT_THROW(future.get(), std::runtime_error);
    
    pool->shutdown();
}

// Test task queue max size enforcement
TEST_F(TestThreadPool, MaxTaskQueueSize) {
    // Use a small max task count
    pool->startup(1, 5);
    
    std::atomic<int> counter = 0;
    std::vector<bool> success(10);
    
    // Submit more tasks than the max queue size
    for (int i = 0; i < 10; i++) {
        success[i] = pool->submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            counter++;
        });
    }
    
    // The first 5 should be accepted, the rest depend on timing
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(success[i]);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // At least the first 5 tasks should have completed
    EXPECT_GE(counter, 5);
    
    pool->shutdown();
}

// Test auto expansion when all workers are busy
TEST_F(TestThreadPool, AutoExpansion) {
    // Start with just 1 thread
    pool->startup(1, 100);
    
    // Variables to track task execution
    std::atomic<int> counter = 0;
    std::mutex mtx;
    std::set<std::thread::id> thread_ids;
    
    // Submit several long-running tasks to trigger expansion
    const int total_tasks = 10;
    for (int i = 0; i < total_tasks; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
        pool->submit([&counter, &mtx, &thread_ids]() {
            std::thread::id this_id = std::this_thread::get_id();
            {
                std::lock_guard<std::mutex> lock(mtx);
				std::cout << this_id << std::endl;
                
                // Record which thread processed this task
				thread_ids.insert(this_id);
            }
            
            // Simulate work - long enough to force thread pool expansion
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            counter++;
        });
    }
    
    // Sleep to allow time for tasks to be processed and pool to expand
    std::this_thread::sleep_for(std::chrono::seconds(11));
    
    // Check that all tasks were completed
    EXPECT_EQ(counter, total_tasks);
    
    // Since we started with 1 thread but had 10 concurrent tasks,
    // the thread pool should have expanded to use more threads
    EXPECT_GT(thread_ids.size(), 1) << "Thread pool should have expanded beyond the initial thread";
    
    pool->shutdown();
}

// Test shutdown with pending tasks
TEST_F(TestThreadPool, ShutdownWithPendingTasks) {
    pool->startup(2, 100);
    
    std::atomic<int> counter = 0;
    
    // Add some long-running tasks
    for (int i = 0; i < 10; i++) {
        pool->submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            counter++;
        });
    }
    
    // Shutdown immediately before tasks complete
    pool->shutdown();
    
    // Counter may be any value from 0 to 10 depending on how many tasks completed before shutdown
    // We don't test its exact value
}

// Test multiple startups without shutdown
TEST_F(TestThreadPool, MultipleStartupsWithoutShutdown) {
    pool->startup(2, 100);
    
    // This shouldn't create additional threads since already started
    pool->startup(4, 200);
    
    std::atomic<bool> task_completed = false;
    pool->submit([&task_completed]() {
        task_completed = true;
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(task_completed);
    
    pool->shutdown();
}

// Test submitting task to stopped pool
TEST_F(TestThreadPool, SubmitToStoppedPool) {
    // Don't start the pool
    
    std::atomic<bool> task_completed = false;
    bool result = pool->submit([&task_completed]() {
        task_completed = true;
    });
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(task_completed);
}

// Test exec on stopped pool
TEST_F(TestThreadPool, ExecOnStoppedPool) {
    // Don't start the pool
    
    auto future = pool->exec([]() {
        return 42;
    });
    
    try {
        future.get();
        FAIL() << "Expected an exception when calling exec on stopped pool";
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ(e.what(), "submit task failed");
    }
}

// Test heavy load
TEST_F(TestThreadPool, HeavyLoad) {
    pool->startup(4, 1000);
    
    std::atomic<int> counter = 0;
    const int num_tasks = 500;
    
    for (int i = 0; i < num_tasks; i++) {
        pool->submit([&counter]() {
            // Small amount of simulated work
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            counter++;
        });
    }
    
    // Wait a bit longer for all tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    EXPECT_EQ(counter, num_tasks);
    
    pool->shutdown();
}

// Test the behavior when thread expansion is disabled
TEST_F(TestThreadPool, NoThreadExpansion) {
    // Start with 2 threads and disable expansion
    pool->startup(2, 100, false);
    
    // Create busy tasks that take time to complete
    std::atomic<int> counter = 0;
    std::mutex mtx;
    std::vector<std::thread::id> thread_ids;
    
    // Submit more tasks than available threads to force task queuing rather than expansion
    const int total_tasks = 20;
    for (int i = 0; i < total_tasks; i++) {
        pool->submit([&counter, &mtx, &thread_ids]() {
            std::thread::id this_id = std::this_thread::get_id();
            {
                std::lock_guard<std::mutex> lock(mtx);
                // Record which thread processed this task
                if (std::find(thread_ids.begin(), thread_ids.end(), this_id) == thread_ids.end()) {
                    thread_ids.push_back(this_id);
                }
            }
            
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            counter++;
        });
    }
    
    // Wait for all tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // All tasks should be processed
    EXPECT_EQ(counter, total_tasks);
    
    // Since expansion is disabled, only the initial 2 threads should have been used
    EXPECT_EQ(thread_ids.size(), 2) << "Only the initial 2 threads should have processed tasks";
    
    pool->shutdown();
}

// Test task count getter functions
TEST_F(TestThreadPool, TaskCountGetters) {
    // Start with specific settings
    const u64 max_tasks = 50;
    pool->startup(2, max_tasks);
    
    // Verify max task count is set correctly
    EXPECT_EQ(pool->get_max_task_count(), max_tasks);
    
    // Initially, there should be no tasks
    EXPECT_EQ(pool->get_curr_task_count(), 0);
    
    // Create a sync point to keep tasks from completing
    std::mutex block_mutex;
    std::condition_variable cv;
    bool ready = false;
    
    // Submit some tasks that wait for a signal
    const int num_tasks = 10;
    for (int i = 0; i < num_tasks; i++) {
        pool->submit([&block_mutex, &cv, &ready]() {
            std::unique_lock<std::mutex> lock(block_mutex);
            cv.wait(lock, [&ready] { return ready; });
        });
    }
    
    // Allow time for tasks to be queued but not yet executed
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    // Check current task count (should reflect the queued tasks)
    EXPECT_EQ(pool->get_curr_task_count(), num_tasks);
    
    // Signal tasks to complete
    {
        std::lock_guard<std::mutex> lock(block_mutex);
        ready = true;
        cv.notify_all();
    }
    
    // Allow time for tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Current task count should be back to 0
    EXPECT_EQ(pool->get_curr_task_count(), 0);
    
    pool->shutdown();
}
