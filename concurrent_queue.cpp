#include "concurrent_queue.h"


template<class T>
void ConcurrentQueue<T>::push(T element) {
    std::unique_lock<std::mutex> locker(deque_mutex);
    data.push_back(element);
    notified = true;
    queue_check.notify_one();
}


template<class T>
T ConcurrentQueue<T>::pop() {
    std::unique_lock<std::mutex> locker(deque_mutex);
    while (!notified) {
        queue_check.wait(locker);
    }
    T a = data.front();
    data.pop_front();
    notified = false;
    return a;
}