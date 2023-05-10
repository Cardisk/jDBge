//
// Created by Matteo Cardinaletti on 03/04/23.
//

#ifndef JDBGE_LOGGER_H
#define JDBGE_LOGGER_H

#include <iostream>

#include "Console.h"

class Logger {
public:
    enum LogType {
        DEBUG = 0,
        RELEASE,
    };

    explicit Logger(const Logger &logger) = delete;
    ~Logger() { delete logger; }

    static Logger &get_instance() {
        if (!logger) {
            logger = new Logger();
        }

        return *logger;
    }

    [[maybe_unused]] void log(const std::string &str) {
        Console::color(Console::Colors::cyan);
        std::cout << "[info] " << str << std::endl;
        Console::color();
    }

    [[maybe_unused]] void error(const std::string &str = "a generic error occurred") {
        Console::color(Console::Colors::red);
        std::cerr << "[error] " << str << std::endl;
        Console::color();
    }

    [[maybe_unused]] void err_n_die(const std::string &str) {
        Console::color(Console::Colors::red);
        std::cerr << "[fatal] " << str << std::endl;
        Console::color();
        ::exit(EXIT_FAILURE);
    }

    [[maybe_unused]] void debug(const std::string &str) {
        if (level != Logger::LogType::DEBUG) return;

        Console::color(Console::Colors::yellow);
        std::cout << "[debug] " << str << std::endl;
        Console::color();
    }

    template <typename T> void debug(const std::vector<T> v) {
        if (level != Logger::LogType::DEBUG) return;

        Console::color(Console::Colors::yellow);
        std::cout << "[debug] ";
        std::cout << "[ ";
        for (size_t i = 0; i < v.size(); i++)
            std::cout << v[i] << (i < v.size() - 1 ? ", " : "");
        std::cout << " ]" << std::endl;
        Console::color();
    }

    void set_level(const Logger::LogType l) {
        level = l;
    }

private:
    Logger() = default;

    inline static Logger *logger = nullptr;
    inline static LogType level = Logger::LogType::RELEASE;
};


#endif //JDBGE_LOGGER_H
