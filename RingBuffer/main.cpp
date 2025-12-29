
#include <iostream> 
#include <nanobench/src/include/nanobench.h>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include "RingBuffer.hpp"
#include "RingBuffer2.hpp"

void runProducerConsumer(auto& buffer)
{
    thread producer([&](){
        for (int i = 0; i < 1'000'000; ++i) {
            while (!buffer.push(i)) {
                continue;
            }
        }
    });

    thread consumer([&](){

        auto consume = [i = 0](int item) mutable {
            // std::cout << "item:  " << item << "i: " << i << "\n";
            assert(item == i++);
        };

        for (int i = 0; i < 1'000'000; ++i) {
            while (!buffer.consume_one(consume)) {
                continue;
            }
        }
    });

    
    producer.join();
    consumer.join();
}


// g++ -std=c++20 -O3 -I../include/boost_1_82_0 -I../include ../include/nanobench.o  main.cpp -o main -Wno-interference-size
int main() {

    boost::lockfree::spsc_queue<int> boost_buffer(1024);
    RingBuffer<int, 1024> my_ring_buffer;
    RingBuffer2<int, 1024> my_ring_buffer2;

    ankerl::nanobench::Bench().minEpochIterations(400).epochs(2).warmup(5).run("my_ring_buffer", [&my_ring_buffer] {
            runProducerConsumer(my_ring_buffer);
        });

    ankerl::nanobench::Bench().minEpochIterations(400).epochs(2).warmup(5).run("boost_spsc", [&boost_buffer] {
            runProducerConsumer(boost_buffer);
        });

    ankerl::nanobench::Bench().minEpochIterations(400).epochs(2).warmup(5).run("my_ring_buffer2", [&my_ring_buffer2] {
            runProducerConsumer(my_ring_buffer2);
        });
}