//
// Created by YuraYeliseev on 2019-04-22.
//

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
    bool notified;
public:
    ConcurrentQueue() = default;
    void push(T element);
    T pop();
};


#endif //PARALLEL_READING_CONCURENT_QUEUE_H
