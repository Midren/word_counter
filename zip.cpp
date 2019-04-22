//
// Created by YuraYeliseev on 2019-04-22.
//

#include "zip.h"

bool Zip::unzip(std::string zipFile, std::string ExtractPath) {
    boost::filesystem::path dir(zipFile);
    if(boost::filesystem::exists(dir)){
        boost::filesystem::path path(ExtractPath);
        boost::filesystem::path currentDir = boost::filesystem::current_path();
        
    }
}