//

#include "concurent_queue.h"


template<typename T>
void ConcurrentQueue<T>::push(T element) {
    std::unique_lock<std::mutex> locker(deque_mutex);
    data.push_back(element);
    notified = true;
    queue_check.notify_one();
}

template<typename T>
T ConcurrentQueue<T>::pop() {
    std::unique_lock<std::mutex> locker(deque_mutex);
    while (!notified) {
        queue_check.wait(locker);
    }
    T a = data.pop_front();
    notified = false;
    return a;
}