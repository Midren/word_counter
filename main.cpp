#include "zip.h"
#include<string>
#include "concurent_queue.h"

int main() {
    ConcurrentQueue<std::string> my_queue;
    my_queue.push(std::string("abc"));
    std::cout << my_queue.pop() << std::endl;
    return 0;
}