#ifndef PARALLEL_READING_CONCURENT_QUEUE_H
#define PARALLEL_READING_CONCURENT_QUEUE_H

#include<iostream>
#include<deque>
#include<condition_variable>
#include<mutex>

template<class T>
class ConcurrentQueue {
private:
    std::deque<T> data;
    std::condition_variable queue_check;
    std::mutex deque_mutex;
public:
    ConcurrentQueue() = default;

    void push(const T &element);

    void double_push(const T &element_1, const T &element_2);

    void push(T &&element);

    std::pair<T, T> double_pop();

    T pop();

    size_t size() {
        std::unique_lock<std::mutex> locker(deque_mutex);
        return data.size();
    }
};

#include "concurrent_queue.cpp"

#endif //PARALLEL_READING_CONCURENT_QUEUE_H
