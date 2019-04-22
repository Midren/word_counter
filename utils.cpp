#include "utils.h"

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
