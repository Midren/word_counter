#ifndef PARALLEL_READING_UTILS_H
#define PARALLEL_READING_UTILS_H

#include <vector>
#include <string>
#include <fstream>
#include "zip.h"
#include <map>
#include <boost/algorithm/string.hpp>

std::string check_input(const std::string &file_name);

std::string get_file_ext(std::string file_name);

#endif //PARALLEL_READING_UTILS_H
