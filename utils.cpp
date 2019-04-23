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

std::string get_file_ext(std::string file_name){
    std::vector<std::string> file_and_ext;
    std::string extension;
    boost::split(file_and_ext, file_name,boost::is_any_of("."));
    return file_and_ext[file_and_ext.size() - 1];
}

std::string check_input(const std::string& file_path){
    std::string my_txt;
    std::string extension = get_file_ext(file_path);
    if (extension == "txt"){
        my_txt = file_path;
    }
    else {
        Zip::unzip(file_path, "../");
        auto arch_items = Zip::list_items(file_path.c_str());
        for(const auto& item: arch_items)
            if (get_file_ext(item) == "txt") {
                my_txt = item;
                break;
            }
    }
    std::ifstream fin("../" + my_txt, std::ifstream::binary);
    std::string data = static_cast<std::ostringstream >(std::ostringstream{} << fin.rdbuf()).str();
    return data;
}
