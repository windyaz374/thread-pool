#include "post.h"

template <class Executor, class Fn> 
void post(Executor &exec, Fn &&func)
{
    using return_type = decltype(func());
    static_assert(std::is_void_v<return_type>, 
                "posting functions with return types must be used with \"use_future\" tag.");
    std::packaged_task<void()> task(std::forward<Fn>(func));
    exec.post(std::move(task));
}

template <class Fn>
constexpr auto useFuture(Fn&& func) {
    return std::make_tuple(use_future_tag{}, std::forward<Fn>(func));
}

template <class Executor, class Fn> 
[[nodiscard]] decltype(auto)
post(Executor& exec, std::tuple<use_future_tag, Fn>&& tpl)
{ 
    using returnType = std::invoke_result_t<Fn>;
    auto&& [_, func] = tpl;
    if constexpr (std::is_void_v<returnType>) 
    {
        std::packaged_task<void()> task(std::move(func));
        auto retFuture = task.get_future();
        exec.post(std::move(task));
        return retFuture;
    }	
    else
    {
        forwardHelper forwarder(std::forward<Fn>(func));	
    	
        auto promise = std::make_shared<std::promise<returnType>>();
        auto retFuture = promise->get_future();

        std::packaged_task<void()> task([promise = std::move(promise), 
                                        forwarder = std::move(forwarder)]() mutable 
        {
            forwarder(promise);
        });
        exec.post(std::move(task));

        return retFuture;
    }   
}
