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

void aocio::line_tokenise(const std::string& line, const std::string& delims, std::vector<std::string>& tokens, bool preserve_delims)
{
    std::string::size_type start_pos = 0;

    while (start_pos < line.size()) {
        auto token_end_pos = line.find_first_of(delims, start_pos); 
        if (token_end_pos == std::string::npos) {
            token_end_pos = line.size();
        }
        std::string token = line.substr(start_pos, token_end_pos - start_pos);
        if (token.size()) {
            tokens.push_back(token);
        }

        if (preserve_delims && token_end_pos != std::string::npos) {
            tokens.push_back(std::string{line[token_end_pos]});
        }

        start_pos = token_end_pos + 1;
    }
}
