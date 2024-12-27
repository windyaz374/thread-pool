#pragma once

#include <tuple>

/**
 * @file post.h
 * @brief This file contains the declaration of the post function.
 */

struct use_future_tag {};

/**
 * @brief A helper function that returns a tuple of use_future_tag and the function.
 * 
 * @param func The function to be wrapped in a tuple.
 * @return The tuple that holds the function and the use_future_tag.
 */
template <class Fn>
constexpr auto useFuture(Fn&& func);

/**
 * @brief Posts or submit a void function to the thread pool.
 * 
 * @param exec The executor.
 * @param func The function to be posted.
 */
template <class Executor, class Fn> 
inline void post(Executor &exec, Fn &&func);

/**
 * @brief Posts or submit a non-void function that returns a value to the thread pool.
 * 
 * @param func The function to be posted.
 * @param std::tuple<use_future_tag, Fn> The tuple that holds the function and the use_future_tag.
 * @return The future object that holds the return value of the function.
 */
template <class Executor, class Fn> [[nodiscard]] 
inline decltype(auto) 
post(Executor& exec, std::tuple<use_future_tag, Fn>&& tpl);