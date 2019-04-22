#ifndef PARALLEL_READING_UTILS_H
#define PARALLEL_READING_UTILS_H

#include <vector>
#include <string>
#include <map>

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

std::vector<std::string> split_to_words(std::string &data);

bool any_str(const std::string &str);

#endif //PARALLEL_READING_UTILS_H
