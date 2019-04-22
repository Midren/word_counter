#include <fstream>
#include <string>
#include <map>

#include "concurrent_queue.h"
#include "utils.h"

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
        snd = queue.pop();
        if (fst.empty() || snd.empty()) {
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
}

