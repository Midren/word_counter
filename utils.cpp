#include "utils.h"

std::string get_file_ext(std::string file_name) {
    std::vector<std::string> file_and_ext;
    std::string extension;
    boost::split(file_and_ext, file_name, boost::is_any_of("."));
    return file_and_ext[file_and_ext.size() - 1];
}

std::string check_input(const std::string &file_path) {
    std::string my_txt;
    std::string extension = get_file_ext(file_path);
    if (extension == "txt") {
        my_txt = file_path;
    } else {
        Zip::unzip(file_path, "../");
        auto arch_items = Zip::list_items(file_path.c_str());
        for (const auto &item: arch_items)
            if (get_file_ext(item) == "txt") {
                my_txt = item;
                break;
            }
    }
    std::ifstream fin("../" + my_txt, std::ifstream::binary);
    std::string data = static_cast<std::ostringstream &>(std::ostringstream{} << fin.rdbuf()).str();
    fin.close();
    boost::filesystem::remove("../" + my_txt);
    return data;
}
