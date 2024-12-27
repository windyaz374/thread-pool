#include <iostream>
#include <boost/asio.hpp>
#include <thread>
//#include <boost/asio/thread_pool.hpp>

int main() {
    boost::asio::thread_pool pool(3);

    for (int i = 0; i < 5; ++i) {
        boost::asio::post(pool, [i]() {
            std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
        });
    }

    std::future<int> r1 = boost::asio::post(pool, boost::asio::use_future([]() { return 2; }));
    std::cout << "Result = " << r1.get() << '\n';

    // Optional: Wait for all tasks to complete
    pool.join();

    return 0;
}