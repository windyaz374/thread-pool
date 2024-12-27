#include "threadPool.h"

threadPool::threadPool(int numThreads = std::thread::hardware_concurrency())
{
    for (int i = 0; i < numThreads; i++)
    {
        m_pool.emplace_back(&threadPool::run, this);
    }
}

threadPool::~threadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_isActive = false;
    }
    m_cv.notify_all();
    for (auto& thread : m_pool)
    {   
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void threadPool::post(std::packaged_task<void()> task)
{
    std::lock_guard lock(m_mutex);
    m_tasks.emplace(std::move(task));
    m_cv.notify_one();
}

void threadPool::run()
{
    while (true)
    {
        thread_local std::packaged_task<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&] () 
            { 
                return !m_isActive || !m_tasks.empty(); 
            });
            if (!m_isActive && m_tasks.empty())
            {
                return;
            }
            task.swap(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}



