
#include <iostream> 
#include "../nanobench/src/include/nanobench.h"
#include <atomic>
// #include <boost/lockfree/spsc_queue.hpp>

// g++ -O3 -I../include ../nanobench/nanobench.o main.cpp -o main
int main() {
    int y = 0;
    std::atomic<int> x(0);
    ankerl::nanobench::Bench().run("compare_exchange_strong", [&] {
        x.compare_exchange_strong(y, 0);
    });
}