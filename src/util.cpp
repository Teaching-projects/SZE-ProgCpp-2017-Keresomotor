#include "util.h"

std::string lower(const std::string& text) {
    std::string lowercase_text;
    std::transform(text.begin(), text.end(), std::back_inserter(lowercase_text), tolower);
    return lowercase_text;
}

std::vector<std::string> split(const std::string& text) {
    std::istringstream stream(text);
    return {std::istream_iterator<std::string>{stream},
            std::istream_iterator<std::string>{}};
}
