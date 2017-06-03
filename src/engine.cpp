#include "engine.h"

std::string Engine::get_input() {
    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void Engine::index(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        if (is_file(path)) {
            index_file(path);
        } else if (is_dir(path)) {
            index_dir(path);
        } else {
            std::cerr << path << " not a file or directory!" << std::endl;
        }
    }
}

std::string Engine::parse_command(const std::string& query) {
    auto tokens = split(lower(query));
    if (tokens.size() > 0) {
        return tokens[0];
    }
    return "";
}

std::vector<std::string> Engine::parse_params(const std::string& query) {
    auto tokens = split(lower(query));
    if (tokens.size() > 0) {
        tokens.erase(tokens.begin());
    }
    return tokens;
}

void Engine::search(const std::vector<std::string>& words) {
    auto documents = index_.intersection(words);
    std::vector<std::pair<double, std::uint32_t>> scores;

    for (const auto& document : documents) {
        scores.push_back(std::make_pair(score(document, words), document));
    }

    std::sort(scores.rbegin(), scores.rend());
    for (const auto& score : scores) {
        std::cout << index_.document_name(score.second) << ": " << score.first << std::endl;
    }
}

double Engine::idf(const std::string& word) {
    double all = index_.count_documents();
    double contains = index_.count_documents(word);
    return std::log(all / (contains + 1)) + 1;
}

void Engine::index_dir(const std::string& path) {
    auto files = list_files(path);
    for (const auto& file : files) {
        index_file(file);
    }
    auto dirs = list_dirs(path);
    for (const auto& dir : dirs) {
        index_dir(dir);
    }
}

void Engine::index_file(const std::string& path) {
    if (index_.add(path)) {
        std::cout << path << " added." << std::endl;
    }
}

bool Engine::is_dir(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

bool Engine::is_file(const std::string& path) const {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode));
}

std::vector<std::string> Engine::list_dirs(const std::string& path) {
    DIR* iterator;
    struct dirent* entry;
    std::vector<std::string> dirs;

    if ((iterator = opendir(path.c_str())) != NULL) {
        while ((entry = readdir(iterator)) != NULL) {
            std::string dir = path + '/' + entry->d_name;
            // '.'-al kezdődő mappák rejtettek, ezeket ignoráljuk.
            if (is_dir(dir) && entry->d_name[0] != '.') {
                dirs.push_back(dir);
            }
        }
        closedir(iterator);
    }
    return dirs;
}

std::vector<std::string> Engine::list_files(const std::string& path) {
    DIR* iterator;
    struct dirent* entry;
    std::vector<std::string> files;

    if ((iterator = opendir(path.c_str())) != NULL) {
        while ((entry = readdir(iterator)) != NULL) {
            std::string file = path + '/' + entry->d_name;
            // '.'-al kezdődő fájlok rejtettek, ezeket ignoráljuk.
            if (is_file(file) && entry->d_name[0] != '.') {
                files.push_back(file);
            }
        }
        closedir(iterator);
    }
    return files;
}

double Engine::score(const std::uint32_t document_id, const std::vector<std::string>& words) {
    double score = 0;
    for (const auto& word : words) {
        score += idf(word) * tf(document_id, word);
    }
    return score;
}

double Engine::tf(const std::uint32_t document_id, const std::string& word) {
    double all = index_.count_words(document_id);
    double contains = index_.count_words(document_id, word);
    return contains / all;
}
