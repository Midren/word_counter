//
// Created by YuraYeliseev on 2019-04-22.
//

#ifndef PARALLEL_READING_ZIP_H
#define PARALLEL_READING_ZIP_H
#include <boost/filesystem.hpp>
#include <iostream>
#include <archive.h>
#include <archive_entry.h>

class Zip {
public:
    Zip() = default;
    static bool unzip(std::string zipFile, std::string ExtractPath);
};


#endif //PARALLEL_READING_ZIP_H
