#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>
#include <type_traits>

/**
 * @file threadPool.h
 * @brief This file contains the declaration of the threadPool class.
 */

class threadPool 
{
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
    std::atomic<bool> m_isActive {true};
    
    /**
    As the thread_pool class only accepts std::packaged_task<void()> type of callable into its queue, I had to create helper struct forwardHelper, a local callable class  which makes the actual function call, stores the return value into a promise object and returns void
    */
    template <class Fn>
    struct forwardHelper
    {
        forwardHelper(Fn&& fn) : task(std::forward<Fn> (fn)) {};
        void operator() (std::shared_ptr<std::promise<returnType>> promise) noexcept
        {
            promise->set_value(task());
        }   
    private:
        std::decay_t<Fn> task;
    };    
};