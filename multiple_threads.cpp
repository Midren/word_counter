#include <fstream>
#include <string>
#include <map>

#include "utils.h"

void
count_words(concurrent_queue &words_queue, concurrent_queue &map_queue) {
    std::vector<std::string> words;
    for (;;) {
        words = words_queue.pop();
        std::map<std::string, size_t> cnt;
        for (auto &word: words)
            ++cnt[word];
        map_queue.push(cnt);
    }
}

void merge_maps(concurent_queue &queue) {
    std::map<std::string, size_t> fst, snd;
    for (;;) {
        fst = queue.pop();
        snd = queue.pop();
        for (auto i = fst.begin(); i != fst.end(); ++i) {
            snd[i.first] += i.second;
        }
        queue.push(snd);
    }
}

int main() {

}

