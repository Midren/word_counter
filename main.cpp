#include "zip.h"
#include "concurrent_queue.h"
#include <thread>
#include <vector>
#include <cmath>
#include <boost/range/iterator_range.hpp>

int main() {
    boost::filesystem::path dir = "../data/";
    boost::filesystem::recursive_directory_iterator it(dir), end;

    std::vector<std::string> files;
    for(auto& entry: boost::make_iterator_range(it, end))
    {
        std::string previous = entry.path().string();
        Zip::unzip(previous, "../tmp/");
    }
    return 0;
}