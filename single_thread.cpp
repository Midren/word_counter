#include "single_thread.h"

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
        if (s.length() > 1 && any_str(s)) {
            words_vec.push_back(move(s));
        }
    }
    return words_vec;
}

std::map<std::string, size_t> count_words(std::vector<std::string> words_vec) {
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
        fout << x.first << " \t:\t" << x.second << std::endl;
    }
}