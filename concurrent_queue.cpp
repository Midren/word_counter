#include "concurrent_queue.h"


template<class T>
void ConcurrentQueue<T>::push(const T &element) {
    std::unique_lock<std::mutex> locker(deque_mutex);
    data.push_back(element);
    locker.unlock();
    queue_check.notify_one();
}

template<class T>
void ConcurrentQueue<T>::push(T &&element) {
    std::unique_lock<std::mutex> locker(deque_mutex);
    data.push_back(std::move(element));
    locker.unlock();
    queue_check.notify_one();
}

template<class T>
T ConcurrentQueue<T>::pop() {
    std::unique_lock<std::mutex> locker(deque_mutex);
    queue_check.wait(locker, [this] { return !data.empty(); });
    T a = std::move(data.front());
    data.pop_front();
    return a;
}

template<class T>
void ConcurrentQueue<T>::double_push(const T &element_1, const T &element_2) {
    std::unique_lock<std::mutex> locker(deque_mutex);
    data.push_back(element_1);
    data.push_back(element_2);
    locker.unlock();
    queue_check.notify_one();
}

template<class T>
std::pair<T, T> ConcurrentQueue<T>::double_pop() {
    std::unique_lock<std::mutex> locker(deque_mutex);
    queue_check.wait(locker, [this] { return data.size() >= 2; });
    T a = std::move(data.front());
    data.pop_front();
    T b = std::move(data.front());
    data.pop_front();
    return std::make_pair(a, b);
}
