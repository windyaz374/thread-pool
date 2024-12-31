#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

/**
 * @file threadPool.h
 * @brief This file contains the declaration of the threadPool class.
 */

class threadPool {
   public:
    explicit threadPool(int numThreads = std::thread::hardware_concurrency());
    ~threadPool();

    /**
     * @brief Posts or submit a void function/task to the thread pool.
     *
     * @param task The task to be posted.
     */
    void post(std::packaged_task<void()> task);

   private:
    /**
     * @brief The main function that runs the thread pool.
     */
    void run() noexcept;

    std::vector<std::thread> m_pool;
    std::queue<std::packaged_task<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_isActive{true};
};