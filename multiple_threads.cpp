#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <thread>
#include <unordered_map>

#include "config_parser.h"

#include "concurrent_queue.h"
#include "utils.h"
#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

#define MAP 1
#ifdef MAP
typedef std::map<std::string, size_t> wMap;
#else
typedef std::unordered_map<std::string, size_t> wMap;
#endif

bool any_str(const std::string &str) {
    size_t num = 0;
    for (auto c : str)
        num += isalpha(c);
    return num > 0;
}

namespace ba=boost::locale::boundary;

void split_to_words(std::string &data, ConcurrentQueue<std::vector<std::string>> &words_queue) {
    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));

    std::vector<std::string> words_vec;
    ba::ssegment_index words(ba::word, data.begin(), data.end());

    constexpr int words_block = 2048;

    for (auto &&x: words) {
        if (x.str().length() >= 1) {
            words_vec.push_back(move(x.str()));
            if (words_vec.size() == words_block) {
                words_queue.push(std::move(words_vec));
                words_vec = std::vector<std::string>();
                words_vec.reserve(words_block);
            }
        }
    }
    if (!words_vec.empty())
        words_queue.push(words_vec);
}

void count_words(ConcurrentQueue<std::vector<std::string>> &words_queue,
                 ConcurrentQueue<wMap> &map_queue) {
    std::vector<std::string> words;
    for (;;) {
        words = words_queue.pop();
        //std::cout << "Counting " << words_queue.size() << std::endl;
        if (words.empty()) {
//            std::cout << "End working " << words.size() << std::endl;
            words_queue.push(std::move(words));
            return;
        }
        //std::cout << "Counting push " << words_queue.size() << std::endl;
        wMap cnt;
        for (auto &word: words) {
            std::string s = boost::locale::fold_case(boost::locale::normalize(word));
            if (s.length() >= 1 && any_str(s)) {
                ++cnt[s];
            }
        }
        if (!cnt.empty())
            map_queue.push(std::move(cnt));
    }
    //std::cout << "End working" << std::endl;
}

void merge_maps(ConcurrentQueue<wMap> &queue) {
    wMap res;
    for (;;) {
//        std::cout << "Merge " << queue.size() << std::endl;
        auto tmp = queue.pop();
//        std::cout << "Merge popped " << queue.size() << std::endl;
        if (tmp.empty()) {
//            std::cout << "Poison pill" << std::endl;
            queue.unlimited_push(std::move(res));
            queue.unlimited_push(std::move(tmp));
            break;
        }
        for (auto &el : tmp) {
            res[el.first] += el.second;
        }
    }
}

#if defined _WIN32
using best_resolution_cpp_clock = std::chrono::steady_clock;
#else
using best_resolution_cpp_clock = std::chrono::high_resolution_clock;
#endif

inline best_resolution_cpp_clock::time_point get_current_wall_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = best_resolution_cpp_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline uint64_t to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}


int main(int argc, char *argv[]) {
    Attributes *a;
    if (argc < 2)
        a = get_intArgs("../config.dat");
    else if (argc == 2) {
        a = get_intArgs(argv[1]);
    } else {
        std::cout << "Invalid input: ./multiple_threads [path to config]" << std::endl;
    }
    ConcurrentQueue<std::vector<std::string>> words_queue;
    ConcurrentQueue<wMap> map_queue;
    wMap count;

    int thread_num = std::stoi(a->NThreads);
    int merge_threads_num = std::floor(thread_num / 3.0);
    std::vector<std::thread> threads(thread_num);
    for (int i = 0; i < thread_num - merge_threads_num; i++)
        threads[i] = std::thread(count_words, std::ref(words_queue), std::ref(map_queue));
    for (int i = thread_num - merge_threads_num; i < thread_num; i++)
        threads[i] = std::thread(merge_maps, std::ref(map_queue));

    std::string dir = "../.tmp/";
    boost::filesystem::create_directory(dir);
    auto start_counting = get_current_wall_time_fenced();
    boost::filesystem::path currentDir = boost::filesystem::current_path();
    unzip_files(boost::filesystem::canonical(dir).string() + "/", boost::filesystem::canonical(a->infile).string());
    boost::filesystem::current_path(currentDir);
    std::cout << "Started counting words!" << std::endl;
    boost::filesystem::recursive_directory_iterator it(dir), end;
    for (auto &entry: boost::make_iterator_range(it, end)) {
        std::string previous = entry.path().string();
        std::string data = check_input(previous);
        split_to_words(data, words_queue);
    }
    boost::filesystem::remove_all(dir);


    std::cout << "Started merging " << std::endl;
    words_queue.push(std::vector<std::string>());
    for (int i = 0; i < thread_num - merge_threads_num; i++)
        threads[i].join();

    map_queue.push(wMap{});
    for (int i = thread_num - merge_threads_num; i < thread_num; i++)
        threads[i].join();
    auto end_counting = get_current_wall_time_fenced();

    auto res = map_queue.pop();
    if (res.empty())
        res = map_queue.pop();
 

    std::vector<std::pair<std::string, size_t>> tmp;
    tmp.reserve(res.size());
    for (auto x: res) {
        tmp.emplace_back(x.first, x.second);
    }
    std::sort(tmp.begin(), tmp.end(), [](auto x, auto y) {
        return x.first < y.first;
    });
    std::ofstream fout1(a->out_by_a);
    for (auto x: tmp) {
        fout1 << x.first << "\t:\t" << x.second << std::endl;
    }
    std::sort(tmp.begin(), tmp.end(), [](auto x, auto y) {
        return x.second > y.second;
    });
    std::ofstream fout2(a->out_by_n);
    for (auto x: tmp) {
        fout2 << x.first << "\t:\t" << x.second << std::endl;
    }

    std::cout << "Counting: " << to_us(end_counting - start_counting) << std::endl;
}

