#ifndef SE_ENGINE_H
#define SE_ENGINE_H

#include <algorithm>
#include <dirent.h>
#include <cmath>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>

#include "index.h"
#include "util.h"

class Engine {
  public:
    Engine();
    void index(const std::vector<std::string>& paths);
    void search(const std::vector<std::string>& words);
    std::string get_input();
    std::string parse_command(const std::string& query);
    std::vector<std::string> parse_params(const std::string& query);
  private:
    void index_dir(const std::string& path);
    void index_file(const std::string& path);
    double score(const std::uint32_t document_id, const std::vector<std::string>& words);
    double idf(const std::string& word);
    double tf(const std::uint32_t document_id, const std::string& word);
    bool is_dir(const std::string& path) const;
    bool is_file(const std::string& path) const;
    std::vector<std::string> list_dirs(const std::string& path);
    std::vector<std::string> list_files(const std::string& path);

    Index index_;
};

#endif
