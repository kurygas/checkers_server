#include "utility_functions.h"

std::vector<std::string> Split(const std::string& str, char splitter) {
    std::vector<std::string> result(1, "");
    for (const auto& c : str) {
        if (c == splitter) {
            result.emplace_back();
        }
        else {
            result.back().push_back(c);
        }
    }
    return result;
}
