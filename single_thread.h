#ifndef PARALLEL_READING_SINGLE_THREAD_H
#define PARALLEL_READING_SINGLE_THREAD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "boost/locale/boundary.hpp"
#include "boost/locale.hpp"

std::vector<std::string> split_to_words(std::string &data);

std::map<std::string, size_t> count_words(std::vector<std::string> words_vec);

#endif //PARALLEL_READING_SINGLE_THREAD_H
