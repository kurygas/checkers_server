#include "utility_functions.h"

std::vector<std::string> Split(const std::string& str) {
    std::vector<std::string> result(1, "");

    for (const auto& c : str) {
        if (c == '$') {
            result.emplace_back();
        }
        else {
            result.back().push_back(c);
        }
    }

    return result;
}
