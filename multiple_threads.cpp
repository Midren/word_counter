#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <thread>

#include "concurrent_queue.h"
#include "utils.h"

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

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
        words_vec.push_back(move(x.str()));
        if (words_vec.size() == words_block) {
            words_queue.push(std::move(words_vec));
            words_vec = std::vector<std::string>();
            words_vec.reserve(words_block);
        }
    }
    words_queue.push(words_vec);
}

void count_words(ConcurrentQueue<std::vector<std::string>> &words_queue,
                 ConcurrentQueue<std::map<std::string, size_t>> &map_queue) {
    std::vector<std::string> words;
    for (;;) {
        words = words_queue.pop();
        if (words.empty()) {
            words_queue.push(words);
            return;
        }
        std::map<std::string, size_t> cnt;
        for (auto &word: words) {
            std::string s = boost::locale::fold_case(boost::locale::normalize(word));
            if (s.length() >= 1 && any_str(s)) {
                ++cnt[s];
            }
        }
        map_queue.push(cnt);
    }
}

void merge_maps(ConcurrentQueue<std::map<std::string, size_t>> &queue) {
    std::map<std::string, size_t> fst, snd;
    for (;;) {
        fst = queue.pop();
        snd = queue.pop();
        if (fst.empty() || snd.empty()) {
            queue.push(fst);
            queue.push(snd);
            if (queue.size() == 2) {
                return;
            } else {
                continue;
            }
        }
        for (auto &el : fst) {
            snd[el.first] += el.second;
        }
        queue.push(snd);
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


int main() {
    ConcurrentQueue<std::vector<std::string>> words_queue;
    ConcurrentQueue<std::map<std::string, size_t >> map_queue;
    std::map<std::string, size_t> count;

    auto start_reading = get_current_wall_time_fenced();
    std::ifstream fin("../data/data.txt", std::ifstream::binary);
    std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << fin.rdbuf()).str();
    auto end_reading = get_current_wall_time_fenced();

    constexpr int thread_num = 2;
    int merge_threads_num = std::floor(thread_num / 4.0);
    std::vector<std::thread> threads(thread_num);

    for (int i = 0; i < thread_num - merge_threads_num; i++)
        threads[i] = std::thread(count_words, std::ref(words_queue), std::ref(map_queue));
    for (int i = thread_num - merge_threads_num; i < thread_num; i++)
        threads[i] = std::thread(merge_maps, std::ref(map_queue));

    auto start_counting = get_current_wall_time_fenced();
    split_to_words(data, words_queue);
    words_queue.push(std::vector<std::string>());

    for (int i = 0; i < thread_num - merge_threads_num; i++) {
        threads[i].join();
        threads[i] = std::thread(merge_maps, std::ref(map_queue));
    }

    map_queue.push(std::map<std::string, size_t>{});
    for (int i = 0; i < thread_num; i++) {
        threads[i].join();
    }
    auto end_counting = get_current_wall_time_fenced();

    auto res = map_queue.pop();

    if (res.empty()) {
        res = map_queue.pop();
    }

    std::ofstream fout("result.txt");
    for (auto x: res) {
        fout << x.first << "\t:\t" << x.second << std::endl;
    }
    auto end_writing = get_current_wall_time_fenced();


    std::cout << "Loading: " << to_us(end_reading - start_reading) << std::endl
              << "Analyzing: " << to_us(end_counting - start_counting) << std::endl
              << "Total: " << to_us(end_writing - start_reading) << std::endl;
}

