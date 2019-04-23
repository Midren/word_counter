#include "zip.h"
#include "concurrent_queue.h"
#include <thread>
#include <vector>
#include <cmath>


int main() {
    Zip::unzip("../data/data.tar", "../");
    return 0;
}