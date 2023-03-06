//
// Created by Matteo Cardinaletti on 04/03/23.
//

#include "Lexer.h"

#include <utility>

Token Lexer::next_token() {
    this->trim_left();
    if (this->content.empty()) {
        return Token("");
    }

    size_t next_token_pos = this->content.find(' ');
    std::string token_text = this->content.substr(0, next_token_pos);
    this->content = this->content.erase(0, next_token_pos);

    return {TokenType::Symbol, token_text};
}

void Lexer::set_content(std::string new_content) {
    this->content = std::move(new_content);
}

std::vector<Token> Lexer::collect() {
    std::vector<Token> tokens = std::vector<Token>();
    Token token = Token("");
    while ((token = this->next_token()).get_type() != TokenType::Invalid) tokens.push_back(token);
    return tokens;
}
