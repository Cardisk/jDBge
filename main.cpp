#include <iostream>
#include <string>

#include "Lexer.h"

void usage(const std::string& err) {
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
    // TODO: interactive shell, user input, query parsing, query execution
}

void clean() {
    // TODO: removing files and folders
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage("invalid program usage");
        ::exit(EXIT_FAILURE);
    }

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
        }
    }

    return EXIT_SUCCESS;
}
