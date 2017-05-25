#include "util.h"

std::string& filter(std::string& text) {
    std::replace_if(text.begin(), text.end(), ispunct, ' ');
    return text;
}

std::string& lower(std::string& text) {
    std::transform(text.begin(), text.end(), text.begin(), tolower);
    return text;
}

std::vector<std::string> split(const std::string& text) {
    std::istringstream stream(text);
    return {std::istream_iterator<std::string>{stream},
            std::istream_iterator<std::string>{}};
}
