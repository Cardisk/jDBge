//
// Created by Matteo Cardinaletti on 04/03/23.
//

#include "Lexer.h"

#include <utility>
#include "Keywords.h"

Token Lexer::next_token() {
    this->trim_left();
    if (this->content.empty()) {
        return Token("");
    }

    size_t next_token_pos = this->content.find(' ');
    std::string token_text = this->content.substr(0, next_token_pos);
    this->content = this->content.erase(0, next_token_pos);

    for (const std::string& word : keywords) {
        if (token_text == word) {
            return {TokenType::Keyword, token_text};
        }
    }

    if (isdigit(token_text[0])) {
        for (const char& c : token_text) {
            if (!isdigit(c)) return {TokenType::Invalid, token_text};
        }
        return {TokenType::Number, token_text};
    }

    if (isalpha(token_text[0]))
        return {TokenType::Symbol, token_text};
    else if (token_text == "==" || token_text == "=")
        return {TokenType::Equal, token_text};
    else if (token_text == "!=")
        return {TokenType::BangEqual, token_text};
    else if (token_text == ">")
        return {TokenType::Greater, token_text};
    else if (token_text == ">=")
        return {TokenType::GreaterEqual, token_text};
    else if (token_text == "<")
        return {TokenType::Less, token_text};
    else if (token_text == "<=")
        return {TokenType::LessEqual, token_text};

    return {TokenType::Invalid, token_text};
}

void Lexer::set_content(std::string new_content) {
    this->content = std::move(new_content);
}

std::vector<Token> Lexer::collect() {
    std::vector<Token> tokens = std::vector<Token>();
    Token token = Token("");
    while ((token = this->next_token()).get_type() != TokenType::E_O_F) tokens.push_back(token);
    return tokens;
}
