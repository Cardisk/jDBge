#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>

#include "components/Lexer.h"

#define vector_print(_VECTOR) \
    do { \
        std::cout << "[ "; \
        for (size_t i = 0; i < (_VECTOR).size(); i++) \
            std::cout << (_VECTOR)[i] << (i < (_VECTOR).size() - 1 ? ", " : ""); \
        std::cout << " ]" << std::endl; \
    } while(0)

void usage(const std::string &err) {
    std::cerr << "jDBge: " << err << std::endl << std::endl;
    std::cerr << "usage: jDBge [SUBCOMMAND] [QUERY]" << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << "\thelp \tprints this message" << std::endl;
    std::cerr << "\tshell\topens interactive mode" << std::endl;
    std::cerr << "\tclean\tremove everything created by this program" << std::endl;
}

std::string shift(int *argc, char ***argv) {
    assert(*argc > 0);
    std::string cmd = std::string(**argv);
    (*argc)--;
    (*argv)++;
    return cmd;
}

void shell() {
    // TODO: query parsing, query execution
    Lexer lexer = Lexer("");
    std::cout << R"(jDBge is running in interactive mode, type "exit" or "quit" to exit the program)" << std::endl;
    while (true) {
        std::string cmd;
        std::cout << ">> ";
        std::getline(std::cin, cmd);

        std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (cmd == "exit" || cmd == "quit") break;

        lexer.set_content(cmd);

        std::vector<Token> tokens = lexer.collect();
        vector_print(tokens);
    }
}

void clean() {
    // TODO: removing files and folders
    assert(0 && "TODO: not implemented yet");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage("invalid program usage");
        ::exit(EXIT_FAILURE);
    }

    shift(&argc, &argv);

    while (argc > 0) {
        std::string cmd = shift(&argc, &argv);
        if (cmd == "help") {
            usage("just barely good enough DBMS");
        } else if (cmd == "shell") {
            shell();                    // TODO: this will open an interactive shell
        } else if (cmd == "clean") {
            clean();                    // TODO: this will remove everything created by this program
        } else {
            usage("unrecognized token '" + cmd + "'");
            ::exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
