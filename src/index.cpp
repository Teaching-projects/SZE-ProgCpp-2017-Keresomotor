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
        terms_[term.first][document_id] += term.second;
    }
    return true;
}

bool Index::load() {
    if (!load_documents()) {
        std::cerr << "documents.se loading unsuccessful!" << std::endl;
        // Sikertelen betöltés esetén ürítjük az indexet, hogy ne maradjon benne szemét.
        documents_.clear();
        terms_.clear();
        return false;
    }
    if (!load_terms()) {
        std::cerr << "terms.se loading unsuccessful!" << std::endl;
        // Sikertelen betöltés esetén ürítjük az indexet, hogy ne maradjon benne szemét.
        documents_.clear();
        terms_.clear();
        return false;
    }
    return true;
}

bool Index::save() {
    if (!save_documents()) {
        std::cerr << "documents.se saving unsuccessful!" << std::endl;
        return false;
    }
    if (!save_terms()) {
        std::cerr << "terms.se saving unsuccessful!" << std::endl;
        return false;
    }
    return true;
}

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

std::string Index::document_name(const std::uint32_t document_id) const {
    if (document_id < documents_.size()) {
        return documents_[document_id].path;
    }
    return "";
}

std::vector<std::uint32_t> Index::intersection(const std::vector<std::string>& words) {
    // Egyes keresett szavak, mely dokumentumokban találhatók meg.
    std::unordered_map<std::uint32_t, std::uint32_t> occurences;
    for (const auto& word : words) {
        if (terms_.find(word) != terms_.end()) {
            for (const auto& doc : terms_[word]) {
                ++occurences[doc.first];
            }
        }
    }

    // Azon dokumentumok kigyűjtése, amelyek az összes keresett szót tartalmazzák.
    std::vector<std::uint32_t> intersection;
    for (const auto& occurence : occurences) {
        if (occurence.second == words.size()) {
            intersection.push_back(occurence.first);
        }
    }
    return intersection;
}

bool Index::load_documents() {
    std::ifstream file("documents.se", std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    try {
        // Letárolt dokumentumok számának beolvasása.
        std::uint32_t documents_count = 0;
        file.read(reinterpret_cast<char*>(&documents_count), sizeof(documents_count));
        documents_.reserve(documents_count);

        for (std::size_t i = 0; i < documents_count; ++i) {
            // Dokumentum útvonalának karakterszámának beolvasása.
            std::uint32_t path_length = 0;
            file.read(reinterpret_cast<char*>(&path_length), sizeof(path_length));

            // Dokumentum útvonalának beolvasása.
            std::string path;
            path.resize(path_length);
            file.read(&path[0], path_length);

            // Dokumentum token számának beolvasása.
            std::uint32_t tokens_count = 0;
            file.read(reinterpret_cast<char*>(&tokens_count), sizeof(tokens_count));

            // Beolvasott adatok hozzáadása az indexhez.
            documents_.push_back({path, tokens_count});
        }
    } catch (std::bad_alloc) {
        return false;
    }
    return file.good();
}

bool Index::load_terms() {
    std::ifstream file("terms.se", std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    try {
        // Mentett termek számának beolvasása.
        std::uint32_t terms_count = 0;
        file.read(reinterpret_cast<char*>(&terms_count), sizeof(terms_count));

        for (std::size_t i = 0; i < terms_count; ++i) {
            // Term karakter számának beolvasása.
            std::uint32_t term_length = 0;
            file.read(reinterpret_cast<char*>(&term_length), sizeof(term_length));

            // Term beolvasása.
            std::string term_name;
            term_name.resize(term_length);
            file.read(&term_name[0], term_length);

            // Termhez tartozó dokumentumok számának beolvasása.
            std::uint32_t documents_count = 0;
            file.read(reinterpret_cast<char*>(&documents_count), sizeof(documents_count));

            for (std::size_t j = 0; j < documents_count; ++j) {
                // Dokumentum id beolvasása.
                std::uint32_t document_id = 0;
                file.read(reinterpret_cast<char*>(&document_id), sizeof(document_id));

                // Dokumentumban adott term előfordulásának számának beolvasása.
                std::uint32_t tokens_count = 0;
                file.read(reinterpret_cast<char*>(&tokens_count), sizeof(tokens_count));

                // Beolvasott adatok hozzáadása az indexhez.
                terms_[term_name][document_id] = tokens_count;
            }
        }
    } catch (std::bad_alloc) {
        return false;
    }
    return file.good();
}

bool Index::save_documents() {
    std::ofstream file("documents.se", std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Indexben lévő dokumentumok számának kiírása.
    std::uint32_t documents_count = documents_.size();
    file.write(reinterpret_cast<const char*>(&documents_count), sizeof(documents_count));

    for (const auto& document : documents_) {
        // Dokumentum útvonalának karakterszámának kiírása.
        std::uint32_t path_length = document.path.size();
        file.write(reinterpret_cast<const char*>(&path_length), sizeof(path_length));

        // Dokumentum útvonalának kiírása.
        file.write(document.path.c_str(), path_length);

        // Dokumentum token számának kiírása.
        file.write(reinterpret_cast<const char*>(&document.tokens_count),
                   sizeof(document.tokens_count));
    }
    return file.good();
}

bool Index::save_terms() {
    std::ofstream file("terms.se", std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Indexben lévő termek számának kiírása.
    std::uint32_t terms_count = terms_.size();
    file.write(reinterpret_cast<const char*>(&terms_count), sizeof(terms_count));

    for (const auto& term : terms_) {
        // Term karakter számának kiírása.
        std::uint32_t term_length = term.first.size();
        file.write(reinterpret_cast<const char*>(&term_length), sizeof(term_length));

        // Term kiírása.
        file.write(term.first.c_str(), term_length);

        // Termhez tartozó dokumentumok számának kiírása.
        std::uint32_t documents_count = term.second.size();
        file.write(reinterpret_cast<const char*>(&documents_count), sizeof(documents_count));

        for (const auto& document : term.second) {
            // Dokumentum id kiírása.
            file.write(reinterpret_cast<const char*>(&document.first), sizeof(document.first));

            // Dokumentumban adott term előfordulásának számának kiírása.
            file.write(reinterpret_cast<const char*>(&document.second), sizeof(document.second));
        }
    }
    return file.good();
}
std::string Index::load_document(const std::string& document_path) {
    std::ifstream file(document_path);
    if (!file.is_open()) {
        return "";
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

std::map<std::string, std::uint32_t> Index::create_terms(const std::vector<std::string>& tokens) {
    std::map<std::string, std::uint32_t> terms;
    for (auto& token : tokens) {
        for (std::size_t length = 1; length <= token.size(); ++length) {
            ++terms[token.substr(0, length)];
        }
    }
    return terms;
}

std::vector<std::string> Index::create_tokens(const std::string& text) {
    return split(filter(lower(text)));
}
