//
// Created by Matteo Cardinaletti on 04/03/23.
//

#ifndef JDBGE_LEXER_H
#define JDBGE_LEXER_H

#include <string>
#include <vector>

#include "Token.h"

class Lexer {
public:
    explicit Lexer(std::string content) : content(std::move(content)) {};

    Token next_token();

    std::vector<Token> collect();

    void set_content(std::string new_content);

private:
    void trim_left() {
        while (!this->content.empty() && this->content[0] == ' ') {
            this->content = this->content.erase(0, 1);
        }
    }

    std::string content;
};

#endif //JDBGE_LEXER_H
