#pragma once

#include <future>

#include "postHelper.h"

/**
 * @file post.h
 * @brief This file contains the declaration of the post function.
 */

/**
 * @brief Posts or submit a void function to the thread pool.
 *
 * @param exec The executor.
 * @param func The function to be posted.
 */
template <class Executor, class Fn>
inline void post(Executor& exec, Fn&& func) {
    using return_type = decltype(func());
    static_assert(std::is_void_v<return_type>,
                  "posting functions with return types must be used with "
                  "\"use_future\" tag.");
    std::packaged_task<void()> task(std::forward<Fn>(func));
    exec.post(std::move(task));
}

/**
 * @brief Posts or submit a non-void function that returns a value to the thread
 * pool.
 *
 * @param func The function to be posted.
 * @param std::tuple<use_future_tag, Fn> The tuple that holds the function and
 * the use_future_tag.
 * @return The future object that holds the return value of the function.
 */
template <class Executor, class Fn>
[[nodiscard]] inline decltype(auto) post(Executor& exec,
                                         std::tuple<use_future_tag, Fn>&& tpl) {
    using returnType = std::invoke_result_t<Fn>;
    auto&& [_, func] = tpl;
    if constexpr (std::is_void_v<returnType>) {
        std::packaged_task<void()> task(std::move(func));
        auto retFuture = task.get_future();
        exec.post(std::move(task));
        return retFuture;
    } else {
        forwardHelper forwarder(std::forward<Fn>(func));

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
