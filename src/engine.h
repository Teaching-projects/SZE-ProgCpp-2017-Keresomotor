#ifndef SE_ENGINE_H
#define SE_ENGINE_H

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "index.h"

class Engine {
public:
    void index(const std::vector<std::string>& paths);
private:
    void index_dir(const std::string& path);
    void index_file(const std::string& path);
    bool is_dir(const std::string& path) const;
    bool is_file(const std::string& path) const;
    std::vector<std::string> list_dirs(const std::string& path);
    std::vector<std::string> list_files(const std::string& path);
private:
    Index index_;
};

#endif
