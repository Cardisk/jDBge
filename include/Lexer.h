//
// Created by Matteo Cardinaletti on 04/03/23.
//

#ifndef JDBGE_LEXER_H
#define JDBGE_LEXER_H

#include <string>
#include <vector>

#include "Token.h"

/// Implementation of a tokenizer.
/// It is capable of splitting strings by space and collect them into a vector of tokens.
class Lexer {
public:
    explicit Lexer(std::string content = "") : content(std::move(content)) {};

    /// Getting the next token from the content of the lexer.
    Token next_token();

    /// Collecting all the tokens into a vector.
    std::vector<Token> collect();

    /// Setting a new content for the lexer.
    void set_content(std::string new_content);

private:
    /// Removing the leading spaces.
    void trim_left() {
        // removing the leading spaces
        while (!this->content.empty() && this->content[0] == ' ') {
            this->content = this->content.erase(0, 1);
        }
    }

    std::string content;
};

#endif //JDBGE_LEXER_H
