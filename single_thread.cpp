#include <map>
#include <string>
#include <fstream>

#include "utils.h"

static std::map<std::string, size_t> count_words(std::vector<std::string> &words_vec) {
    std::map<std::string, size_t> words_counter;
    for (auto &&w: words_vec) {
        words_counter[w] += 1;
    }
    return words_counter;
}

int main() {
    std::ifstream fin("../data/data.txt", std::ifstream::binary);
    std::ofstream fout("result.txt");
    std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << fin.rdbuf()).str();

    auto words_vec = split_to_words(data);
    auto words_counter = count_words(words_vec);

    for (auto x: words_counter) {
        fout << x.first << "\t:\t" << x.second << std::endl;
    }
}