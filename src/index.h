#ifndef SE_INDEX_H
#define SE_INDEX_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "util.h"

struct Document {
  public:
    std::string path;
    std::uint32_t tokens_count;
};

class Index {
  public:
    bool add(const std::string& document_path);
    bool load();
    bool save();
    bool contains(const std::string& document_path) const;
    std::uint32_t count_documents() const;
    std::uint32_t count_documents(const std::string& word) const;
    std::uint32_t count_words(const std::uint32_t document_id) const;
    std::uint32_t count_words(const std::uint32_t document_id, const std::string& word) const;
    std::string document_name(const std::uint32_t document_id) const;
    std::vector<std::uint32_t> intersection(const std::vector<std::string>& words);

  private:
    bool load_documents();
    bool load_terms();
    bool save_documents();
    bool save_terms();
    std::string load_document(const std::string& document_path);
    std::map<std::string, std::uint32_t> create_terms(const std::vector<std::string>& tokens);
    std::vector<std::string> create_tokens(const std::string& text);

    std::vector<Document> documents_;
    std::unordered_map<std::string, std::map<std::uint32_t, std::uint32_t>> terms_;
};

#endif
