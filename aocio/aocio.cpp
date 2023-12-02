#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>

#include "aocio.hpp"

bool aocio::file_getlines(std::string_view fname, std::vector<std::string>& lines)
{
    std::ifstream file {fname};
    if (!file) {
        std::cerr << "Cannot open file " << fname << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return true;
}
