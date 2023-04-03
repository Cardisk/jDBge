#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>

#include "components/Console.h"
#include "components/Lexer.h"
#include "components/Parser.h"
#include "components/VM.h"
#include "components/Logger.h"

#define vector_print(_VECTOR) \
    do { \
        std::cout << "[ "; \
        for (size_t i = 0; i < (_VECTOR).size(); i++) \
            std::cout << (_VECTOR)[i] << (i < (_VECTOR).size() - 1 ? ", " : ""); \
        std::cout << " ]" << std::endl; \
    } while(0)

/// Printing the usage with a custom error message.
/// \param err Error message
void usage(const std::string &err) {
    std::cerr << "jDBge: " << err << std::endl << std::endl;
    std::cerr << "usage: jDBge [SUBCOMMAND] [QUERY]" << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << "\thelp \tprints this message" << std::endl;
    std::cerr << "\tshell\topens interactive mode" << std::endl;
    std::cerr << "\tclean\tremove everything created by this program" << std::endl;
}

/// Shifting the command line arguments.
/// \param argc Main argc
/// \param argv Main argv
/// \return String representing the token
std::string shift(int *argc, char ***argv) {
    assert(*argc > 0);
    std::string cmd = std::string(**argv);
    (*argc)--;
    (*argv)++;
    return cmd;
}

/// Interactive shell.
void shell() {
    Lexer lexer = Lexer();
    Parser parser = Parser();
    VM vm = VM();
    Logger &logger = Logger::get_instance();
    logger.set_level(Logger::LogType::DEBUG);

    // raw string as a header
    std::cout << R"(jDBge is running in interactive mode, type "exit" or "quit" to exit the program)" << std::endl;
    while (true) {
        std::string cmd;
        Console::getty(cmd, ">> ");

        // setting the new command to lowercase
        std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        // quitting commands
        if (cmd == "exit" || cmd == "quit" || !std::cin) break;

        // tokenizing with the lexer
        lexer.set_content(cmd);
        std::vector<Token> tokens = lexer.collect();

        // vector_print(tokens);

        // parsing with the parser
        parser.set_tokens(tokens);
        Query query = parser.compile_query();

        // if (query != EMPTY_QUERY) std::cout << query << std::endl;
        vm.exec_query(query);
    }
}

/// Garbage man.
void clean() {
    // TODO: removing files and folders
    assert(0 && "TODO: not implemented yet");
}

int main(int argc, char **argv) {
    // die if the user provide bad stuff
    if (argc < 2) {
        usage("invalid program usage");
        ::exit(EXIT_FAILURE);
    }

    // dropping program name
    shift(&argc, &argv);

    // interpreting command arguments
    while (argc > 0) {
        // getting next token
        std::string cmd = shift(&argc, &argv);
        if (cmd == "help") {
            // printing the usage and die
            usage("just barely good enough DBMS");
        } else if (cmd == "shell") {
            // going inside the interactive shell
            shell();
        } else if (cmd == "clean") {
            // cleaning the program stuff and die
            clean();                    // TODO: this will remove everything created by this program
        } else {
            // wrong usage
            usage("unrecognized token '" + cmd + "'");
            ::exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
