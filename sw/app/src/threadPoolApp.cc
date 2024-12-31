#include "post.h"
#include "threadPool.h"

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
