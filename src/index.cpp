#include "index.h"

bool Index::add(const std::string& document_path) {
    if (contains(document_path)) {
        std::cerr << document_path << " already in the index!" << std::endl;
        return false;
    }

    auto text = load_document(document_path);
    if (text.empty()) {
        std::cerr << document_path << " cannot be opened or empty!" << std::endl;
        return false;
    }
    auto tokens = create_tokens(text);
    auto terms = create_terms(tokens);

    std::uint32_t document_id = documents_.size();
    documents_.push_back({document_path, static_cast<std::uint32_t>(tokens.size())});
    for (auto& term : terms) {
        ++terms_[term][document_id];
    }

    return true;
}

//TODO: Optimalizálni!
bool Index::contains(const std::string& document_path) const {
    for (auto& document : documents_) {
        if (document.path.compare(document_path) == 0) {
            return true;
        }
    }
    return false;
}


std::uint32_t Index::count_documents() const {
    return documents_.size();
}

std::uint32_t Index::count_documents(const std::string& word) const {
    auto term = terms_.find(word);
    if (term != terms_.end()) {
        return term->second.size(); 
    }
    return 0;
}

std::uint32_t Index::count_words(const std::uint32_t document_id) const {
    if (document_id < documents_.size()) {
        return documents_[document_id].tokens_count;
    }
    return 0;
}

std::uint32_t Index::count_words(const std::uint32_t document_id, const std::string& word) const {
    auto term = terms_.find(word);
    if (term != terms_.end()) {
        auto document = term->second.find(document_id);
        if (document != term->second.end()) {
            return document->second;
        }
    }
    return 0;
}

std::string Index::load_document(const std::string& document_path) {
    std::fstream file(document_path);
    if (!file.is_open()) {
        return "";
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

std::vector<std::string> Index::create_terms(const std::vector<std::string>& tokens) {
    std::vector<std::string> terms;
    for (auto& token : tokens) {
        for (std::size_t length = 2; length <= token.size(); ++length) {
           terms.push_back(token.substr(0, length));
        }
    }
    return terms;
}

std::vector<std::string> Index::create_tokens(const std::string& text) {
    return split(filter(lower(text)));
}
