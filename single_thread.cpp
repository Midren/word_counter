#include <map>
#include <string>
#include <fstream>
#include <chrono>

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

namespace ba=boost::locale::boundary;

bool any_str(const std::string &str) {
    size_t num = 0;
    for (auto c : str)
        num += isalpha(c);
    return num > 0;
}

std::vector<std::string> split_to_words(std::string &data) {
    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));

    std::vector<std::string> words_vec;
    ba::ssegment_index words(ba::word, data.begin(), data.end());
    for (auto &&x: words) {
        std::string s = boost::locale::fold_case(boost::locale::normalize(x.str()));
        if (s.length() >= 1 && any_str(s)) {
            words_vec.push_back(move(s));
        }
    }
    return words_vec;
}

static std::map<std::string, size_t> count_words(std::vector<std::string> &words_vec) {
    std::map<std::string, size_t> words_counter;
    for (auto &&w: words_vec) {
        words_counter[w] += 1;
    }
    return words_counter;
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
    auto start_reading = get_current_wall_time_fenced();
    std::ifstream fin("../data/data.txt", std::ifstream::binary);
    std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << fin.rdbuf()).str();
    auto end_reading = get_current_wall_time_fenced();

    auto start_counting = get_current_wall_time_fenced();
    auto words_vec = split_to_words(data);
    auto words_counter = count_words(words_vec);
    auto end_counting = get_current_wall_time_fenced();

    std::ofstream fout("result.txt");
    for (auto x: words_counter) {
        fout << x.first << "\t:\t" << x.second << std::endl;
    }
    auto end_writing = get_current_wall_time_fenced();

    std::cout << "Loading: " << to_us(end_reading - start_reading) << std::endl
              << "Analyzing: " << to_us(end_counting - start_counting) << std::endl
              << "Total: " << to_us(end_writing - start_reading) << std::endl;
}