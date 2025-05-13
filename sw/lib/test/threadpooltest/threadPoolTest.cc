#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>

#include "post.h"
#include "postHelper.h"
#include "threadPool.h"

class ThreadPoolTest : public ::testing::Test {
   protected:
    void SetUp() override {
        pool = std::make_unique<threadPool>(NUM_TEST_THREADS);
    }

    void TearDown() override { pool.reset(); }

    static constexpr int NUM_TEST_THREADS{4};
    static constexpr int NUM_TASKS{5};
    std::unique_ptr<threadPool> pool;
};

TEST_F(ThreadPoolTest, ConstructorTest) {
    // Test constructor with different number of threads
    threadPool pool1(2);
    threadPool pool2(8);
    threadPool pool3;  // Default constructor
}

TEST_F(ThreadPoolTest, BasicVoidTaskTest) {
    std::atomic<bool> taskExecuted{false};

    // Post a simple void task like in threadPoolApp.cc
    post(*pool, [&taskExecuted]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        taskExecuted = true;
    });

    // Wait a bit and verify task executed
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_TRUE(taskExecuted);
}

TEST_F(ThreadPoolTest, MultipleVoidTasksTest) {
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;

    // Post multiple void tasks and get futures
    for (int i = 0; i < NUM_TASKS; ++i) {
        auto future = post(*pool, useFuture([&counter]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            counter++;
        }));
        futures.push_back(std::move(future));
    }

    // Wait for all futures to complete
    for (auto& future : futures) {
        future.wait();
    }

    EXPECT_EQ(counter.load(), NUM_TASKS);
}

TEST_F(ThreadPoolTest, NonVoidTaskTest) {
    // Test non-void task with useFuture like in threadPoolApp.cc
    auto future = post(*pool, useFuture([]() -> int {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 42;
    }));

    EXPECT_EQ(future.get(), 42);
}

TEST_F(ThreadPoolTest, MixedTasksTest) {
    std::atomic<int> voidCounter{0};
    std::vector<std::future<int>> nonVoidFutures;
    std::vector<std::future<void>> voidFutures;

    // Mix of void and non-void tasks
    for (int i = 0; i < NUM_TASKS; ++i) {
        if (i % 2 == 0) {
            // Non-void task
            auto future = post(*pool, useFuture([i]() -> int {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return i;
            }));
            nonVoidFutures.push_back(std::move(future));
        } else {
            // Void task
            auto future = post(*pool, useFuture([&voidCounter]() {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                voidCounter++;
            }));
            voidFutures.push_back(std::move(future));
        }
    }

    // Verify non-void results
    for (size_t i = 0; i < nonVoidFutures.size(); ++i) {
        EXPECT_EQ(nonVoidFutures[i].get(), i * 2);
    }

    // Wait for all void futures to complete
    for (auto& future : voidFutures) {
        future.wait();
    }
    // Verify void task counter
    EXPECT_EQ(voidCounter.load(), NUM_TASKS / 2);
}

TEST_F(ThreadPoolTest, DestructorTest) {
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    {
        threadPool localPool(NUM_TEST_THREADS);

        // Post tasks that will be pending when destructor runs
        for (int i = 0; i < NUM_TASKS; ++i) {
            auto future =
                post(localPool, useFuture([&counter]() {
                         std::this_thread::sleep_for(std::chrono::seconds(1));
                         counter++;
                     }));
            futures.push_back(std::move(future));
        }
        // Destructor should wait for tasks to complete
    }

    // Wait for all futures to complete
    for (auto& future : futures) {
        future.wait();
    }

    EXPECT_EQ(counter.load(), NUM_TASKS);
}