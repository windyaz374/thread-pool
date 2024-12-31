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

class threadPool {
   public:
    explicit threadPool(int numThreads = std::thread::hardware_concurrency()) {
        for (int i = 0; i < numThreads; i++) {
            m_pool.emplace_back(&threadPool::run, this);
        }
    }

    ~threadPool() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_isActive = false;
        }
        m_cv.notify_all();
        for (auto& thread : m_pool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void post(std::packaged_task<void()> task) {
        std::lock_guard lock(m_mutex);
        m_tasks.emplace(std::move(task));
        m_cv.notify_one();
    }

   private:
    void run() noexcept {
        while (true) {
            thread_local std::packaged_task<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock,
                          [&]() { return !m_isActive || !m_tasks.empty(); });
                if (!m_isActive && m_tasks.empty()) {
                    return;
                }
                task.swap(m_tasks.front());
                m_tasks.pop();
            }
            task();
        }
    }

    std::vector<std::thread> m_pool;
    std::queue<std::packaged_task<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_isActive{true};
};

template <class Executor, class Fn>
void post(Executor& exec, Fn&& func) {
    using return_type = decltype(func());
    static_assert(std::is_void_v<return_type>,
                  "posting functions with return types must be used with "
                  "\"use_future\" tag.");
    std::packaged_task<void()> task(std::forward<Fn>(func));
    exec.post(std::move(task));
}

struct use_future_tag {};
template <class Fn>
constexpr auto useFuture(Fn&& func) {
    return std::make_tuple(use_future_tag{}, std::forward<Fn>(func));
}

template <class Executor, class Fn>
[[nodiscard]] decltype(auto) post(Executor& exec,
                                  std::tuple<use_future_tag, Fn>&& tpl) {
    using returnType = std::invoke_result_t<Fn>;
    auto&& [_, func] = tpl;
    if constexpr (std::is_void_v<returnType>) {
        std::packaged_task<void()> task(std::move(func));
        auto retFuture = task.get_future();
        exec.post(std::move(task));
        return retFuture;
    } else {
        struct forwardHelper {
            forwardHelper(Fn&& fn) : task(std::forward<Fn>(fn)){};
            void operator()(
                std::shared_ptr<std::promise<returnType>> promise) noexcept {
                promise->set_value(task());
            }

           private:
            std::decay_t<Fn> task;
        } forwarder(std::forward<Fn>(func));

        auto promise = std::make_shared<std::promise<returnType>>();
        auto retFuture = promise->get_future();

        std::packaged_task<void()> task(
            [promise = std::move(promise),
             forwarder = std::move(forwarder)]() mutable {
                forwarder(promise);
            });
        exec.post(std::move(task));

        return retFuture;
    }
}

using namespace std::chrono_literals;
int main() {
    threadPool pool(5);

    for (int i = 0; i < 5; ++i) {
        post(pool, [i]() {
            std::this_thread::sleep_for(1s);
            std::cout << "Task " << i << " executed by thread "
                      << std::this_thread::get_id() << std::endl;
        });
    }

    auto waiter = post(pool, useFuture([]() -> int {
                           std::this_thread::sleep_for(1s);
                           return 42;
                       }));
    std::cout << "Result = " << waiter.get() << '\n';

    return 0;
}
