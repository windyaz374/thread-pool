#pragma once

#include <future>
#include <tuple>

/**
As the thread_pool class only accepts std::packaged_task<void()> type of
callable into its queue, I had to create helper struct forwardHelper, a local
callable class  which makes the actual function call, stores the return value
into a promise object and returns void
*/
template <class Fn>
struct forwardHelper {
    using returnType = std::invoke_result_t<Fn>;
    forwardHelper(Fn&& fn) : task(std::forward<Fn>(fn)){};
    void operator()(
        std::shared_ptr<std::promise<returnType>> promise) noexcept {
        promise->set_value(task());
    }

   private:
    std::decay_t<Fn> task;
};

struct use_future_tag {};

/**
 * @brief A helper function that returns a tuple of use_future_tag and the
 * function.
 *
 * @param func The function to be wrapped in a tuple.
 * @return The tuple that holds the function and the use_future_tag.
 */
template <class Fn>
constexpr auto useFuture(Fn&& func) {
    return std::make_tuple(use_future_tag{}, std::forward<Fn>(func));
}