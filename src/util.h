#ifndef SE_UTIL_H
#define SE_UTIL_H

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string& filter(std::string& text);
std::string& lower(std::string& text);
std::vector<std::string> split(const std::string& text);

#endif
