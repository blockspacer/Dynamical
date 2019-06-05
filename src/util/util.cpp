#include "util.h"

#include <iostream>
#include <fstream>

std::vector<char> Util::readFile(const char* filename) {
    
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    return result;
    
}


