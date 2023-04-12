//
// Created by Matteo Cardinaletti on 30/03/23.
//

#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <deque>
#include <termios.h>
#include <unistd.h>

class BufferToggle {
public:
    explicit BufferToggle() : t() {}

    // Disables buffered input
    void off() {
        tcgetattr(STDIN_FILENO, &t); // get the current terminal I/O structure
        t.c_lflag &= ~ICANON; // Manipulate the flag bits to do what you want it to do
        t.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, 0, &t); // Apply the new settings
    }

    // Enables buffered input
    void on() {
        tcgetattr(STDIN_FILENO, &t); // get the current terminal I/O structure
        t.c_lflag |= ICANON; // Manipulate the flag bits to do what you want it to do
        t.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, 0, &t); // Apply the new settings
    }

private:
    struct termios t;

};

class Console {
public:
    enum Keys {
        macos_first_arrow = 27,
        first_printable = 32,
        macos_second_arrow = 91,
        last_printable = 126,
        backspace = 127,
    };

    enum Colors {
        base = 0,
        black = 30,
        red = 31,
        green = 32,
        yellow = 33,
        blue = 34,
        magenta = 35,
        cyan = 36,
        white = 37,
    };

    static void getty(std::string &str, const std::string &prompt) {
        str.clear();
        BufferToggle tg;
        tg.off();

        int ch;
        int cursor = 0;
        bool del = false;
        size_t flush = 0;
        auto it = Console::history.end();

        std::cout << prompt;
        do {
            ch = getchar();

            if (ch >= Keys::first_printable && ch <= Keys::last_printable) {
                if (!cursor) str.push_back((char) ch);
                else str.insert(str.size() - cursor, 1, (char) ch);
            } else {
                switch (ch) {
                    case Keys::macos_first_arrow:
                        int arrow[2];
                        arrow[0] = getchar();
                        arrow[1] = getchar();
                        if (arrow[0] == Keys::macos_second_arrow && (arrow[1] >= 65 && arrow[1] <= 68)) {
                            switch (arrow[1]) {
                                case 65:    // up
                                    if (Console::has_history && !Console::history.empty()) {
                                        cursor = 0;
                                        flush = str.size() + prompt.size();
                                        if (it != Console::history.begin()) it--;
                                        str = *it;
                                    }
                                    break;
                                case 66:    // down
                                    if (Console::has_history && !Console::history.empty()) {
                                        cursor = 0;
                                        flush = str.size() + prompt.size();
                                        if (it != Console::history.end()) it++;
                                        str = *it;
                                    }
                                    break;
                                case 67:    // right
                                    (cursor > 0) ? cursor-- : 0;
                                    break;
                                case 68:    // left
                                    (cursor < str.size()) ? cursor++ : 0;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case Keys::backspace:
                        if (!str.empty()) {
                            del = true;
                            str.erase((str.size() - 1) - cursor, 1);
                        }
                        break;
                    default:
                        break;
                }
            }

            if (Console::echo) {
                std::cout << "\r";
                for (int i = 0; i < flush; ++i) std::cout << " ";
                std::cout << "\r" << prompt << str;
                if (del) std::cout << " \b";
                for (int i = 0; i < cursor; ++i) std::cout << "\b";
            }
        } while (ch != '\n');

        if (Console::has_history && !str.empty()) {
            Console::history.push_back(str);
            if (Console::history.size() > 50) Console::history.pop_front();
        }
        std::cout << std::endl;
        tg.on();
    }

    static void print_history() {
        std::cout << "[ ";
        for (auto it = Console::history.begin(); it != Console::history.end(); it++) {
            std::cout << *it;
            if (it != Console::history.end() - 1) std::cout << ", ";
        }
        std::cout << " ]" << std::endl;
    }

    static void color(Console::Colors color = Console::Colors::base) {
        std::cout << "\033[0;" << color << "m";
    }

    // should be set to false if you want some sort of password field
    inline static bool echo = true;
    // should be set to true if you want to implement a history
    inline static bool has_history = true;
    inline static std::deque<std::string> history = {};
};

#endif //CONSOLE_H
