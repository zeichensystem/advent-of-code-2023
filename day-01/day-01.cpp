#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>
#include <cassert>

/*
    Notes:
        - It would probably be less ugly to walk from the start and end of the string at the same time to meet in the middle
        - For part 2: Don't increase the iteration variable depending on the word.size() if the word was matched 
                      (edge case which would make this fail here: "twone" is both two and one)
    Solutions:
        - part 1: 55712
        - part 2: 55413
*/

static bool file_getlines(std::string_view fname, std::vector<std::string>& lines)
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

static int64_t solve_part(const std::vector<std::string>& lines, int part_n)
{
    static constexpr int DIGIT_UNDEFINED = 12345;
    int64_t total_sum = 0; 
    std::vector<std::string> nmbr_words = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    for (const auto &line : lines) {
        int first = DIGIT_UNDEFINED, last = DIGIT_UNDEFINED;
        int first_idx = line.size(), last_idx = -1;

        for (int i = 0; i < static_cast<int>(line.size()); ++i) { // For each character of the current line, do 1.) and 2.)
            char c = line.at(i);
            // 1.) Consider the simple single-char digits (for both part 1 and part 2):
            if (c >= '0' && c <= '9') {
                int digit = c - '0';
                if (first == DIGIT_UNDEFINED) {
                    first = digit; 
                    first_idx = i;
                }
                last = digit; 
                last_idx = i;
            } 
            if (part_n == 1) {
                continue;
            }
            // 2.) Consider the number-words (only for part 2):
            for (const std::string& word : nmbr_words) { 
                int word_as_num = &word - &nmbr_words[0] + 1; // The current index; plus one to get the integer of the current number-word.
                std::string::size_type start_idx = line.find(word, i);
                int start_idx_int = static_cast<int>(start_idx);
                if (start_idx == std::string::npos || start_idx_int > i) { // word was not found or does not start at the current index i. 
                    continue;
                }
                if (start_idx_int < first_idx) {
                    first = word_as_num;
                    first_idx = start_idx_int;
                } else if (start_idx_int > last_idx) {
                    last = word_as_num;
                    last_idx = start_idx_int;
                }
            }
        }
        // Use the calculated digits of the current line, convert them to a number and add it to the total sum. 
        assert(first != DIGIT_UNDEFINED && last != DIGIT_UNDEFINED);
        int line_sum = first * 10 + last; 
        total_sum += line_sum;
    }
    return total_sum;
}

int main()
{
    std::vector<std::string> lines;
    bool file_read = file_getlines("../day-01/input.txt", lines);
    assert(file_read);

    std::cout << "Part one: " <<  solve_part(lines, 1) << "\n";
    std::cout << "Part two: " <<  solve_part(lines, 2) << "\n";

    return 0;
}