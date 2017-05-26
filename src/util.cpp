#include "util.h"

std::string filter(const std::string& text) {
    std::string result = text;
    std::replace_if(result.begin(), result.end(), ::ispunct, ' ');
    return result;
}

std::string lower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> split(const std::string& text) {
    std::istringstream stream(text);
    return {std::istream_iterator<std::string>{stream},
            std::istream_iterator<std::string>{}};
}
