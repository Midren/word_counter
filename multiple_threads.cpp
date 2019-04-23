#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <thread>

#include "concurrent_queue.h"

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"
#include "utils.h"

namespace ba=boost::locale::boundary;

void split_to_words(std::string &data, ConcurrentQueue<std::vector<std::string>> &words_queue) {
    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));

    std::vector<std::string> words_vec;
    ba::ssegment_index words(ba::word, data.begin(), data.end());

    for (auto &&x: words) {
        std::string s = boost::locale::fold_case(boost::locale::normalize(x.str()));
        if (s.length() > 1 && any_str(s)) {
            words_vec.push_back(move(s));
            if (words_vec.size() == 1024) {
                //TODO: Add move
                words_queue.push(words_vec);
                words_vec.clear();
            }
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
        for (auto &word: words)
            ++cnt[word];
        map_queue.push(cnt);
    }
}

void merge_maps(ConcurrentQueue<std::map<std::string, size_t>> &queue) {
    std::map<std::string, size_t> fst, snd;
    for (;;) {
        fst = queue.pop();
        if (fst.empty()) {
            queue.push(fst);
            return;
        }
        snd = queue.pop();
        if (snd.empty()) {
            queue.push(fst);
            queue.push(snd);
            return;
        }
        for (auto i = fst.begin(); i != fst.end(); ++i) {
            snd[i->first] += i->second;
        }
        queue.push(snd);
    }
}

int main() {
    ConcurrentQueue<std::vector<std::string>> words_queue;
    ConcurrentQueue<std::map<std::string, size_t >> map_queue;
    std::map<std::string, size_t> count;

    std::ifstream fin("../data/data.txt", std::ifstream::binary);
    std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << fin.rdbuf()).str();

    constexpr int thread_num = 8;
    int merge_threads_num = std::ceil(thread_num / 4.0);
    std::vector<std::thread> threads(thread_num);
    for (int i = 0; i < thread_num - merge_threads_num; i++) {
        threads[i] = std::thread(count_words, std::ref(words_queue), std::ref(map_queue));
    }
    for (int i = thread_num - merge_threads_num; i < thread_num; i++) {
        threads[i] = std::thread(merge_maps, std::ref(map_queue));
    }
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
    auto res = map_queue.pop();
    if (res.size() == 0) {
        res = map_queue.pop();
    }

    std::ofstream fout("result.txt");
    for (auto x: res) {
        fout << x.first << "\t:\t" << x.second << std::endl;
    }
}

