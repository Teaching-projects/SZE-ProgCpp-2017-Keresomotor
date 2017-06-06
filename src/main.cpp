#include <string>

#include "engine.h"

int main() {
    Engine engine;
    bool run = true;

    std::cout << "For help, type \"help\"." << std::endl;
    while (run) {
        auto query = engine.get_input();
        auto command = engine.parse_command(query);
        auto params = engine.parse_params(query);

        if (command.compare("index") == 0) {
            if (params.size() > 0) {
                engine.index(params);
            } else {
                std::cerr << "  INDEX path [path...]" << std::endl;
            }
        } else if (command.compare("search") == 0) {
            if (params.size() > 0) {
                engine.search(params);
            } else {
                std::cerr << "  SEARCH keyword [keyword...]" << std::endl;
            }
        } else if (command.compare("help") == 0) {
            std::cout << "INDEX path [path...] " << std::endl;
            std::cout << "  Summary: Add file(s) or directory(ies) to the index to search on them." << std::endl;
            std::cout << "  Example: index file1.txt dir1/ file2 dir2" << std::endl;
            std::cout << "SEARCH keyword [keyword...]" << std::endl;
            std::cout << "  Summary: Search for keyword(s) among the indexed files." << std::endl;
            std::cout << "  Example: search hello world" << std::endl;
            std::cout << "HELP" << std::endl;
            std::cout << "  Summary: Display list of commands." << std::endl;
            std::cout << "EXIT" << std::endl;
            std::cout << "  Summary: Exit program." << std::endl;
        } else if (command.compare("exit") == 0) {
            run = false;
        } else {
            std::cerr << "Invalid command!" << std::endl;
            std::cerr << "Type \"help\" for the available commands." << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
