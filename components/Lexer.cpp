//
// Created by Matteo Cardinaletti on 04/03/23.
//

#include "Lexer.h"

#include <utility>
#include "Keywords.h"

Token Lexer::next_token() {
    // trimming the content
    this->trim_left();
    if (this->content.empty()) {
        return Token();
    }

    // getting the substring and erasing it from the lexer content
    size_t next_token_pos = this->content.find(' ');
    std::string token_text = this->content.substr(0, next_token_pos);
    this->content = this->content.erase(0, next_token_pos);

    // tokenizing symbols like foo:"bar baz" to work with insert queries
    if (token_text.contains("\"")) {
        next_token_pos = this->content.find('"');
        token_text += this->content.substr(0, next_token_pos + 1);
        this->content = this->content.erase(0, next_token_pos + 1);
    }

    // tokenizing string literals with spaces inside them
    if (token_text[0] == '"' && !this->content.empty()) {
        size_t string_terminator = this->content.find('"');
        token_text += this->content.substr(0, string_terminator + 1);
        this->content = this->content.erase(0, string_terminator + 1);
    }

    // this will check only if the token can be a possible meta_cmd
    if (token_text[0] == '.')
        return {TokenType::Meta_cmd, token_text};

    // checking if the token is a keyword
    for (const std::string& word : keywords) {
        if (token_text == word) {
            return {TokenType::Keyword, token_text};
        }
    }

    // checking if the token is a number
    if (isdigit(token_text[0])) {
        for (const char& c : token_text) {
            if (!isdigit(c)) return {TokenType::Invalid, token_text};
        }
        return {TokenType::Number, token_text};
    }

    // checking if the token is a string literal
    if (token_text.length() > 2 && token_text.starts_with('"') && token_text.ends_with('"')) {
        token_text = token_text.erase(0, 1);
        token_text = token_text.erase(token_text.length() - 1, token_text.length());

        return {TokenType::Symbol, token_text};
    } else if (!token_text.ends_with('"')) {
        // checking if the token is a symbol
        if (isalpha(token_text[0]))
            return {TokenType::Symbol, token_text};
        // checking if the token is an is_equal operator
        else if (token_text == "==" || token_text == "=")
            return {TokenType::Equal, token_text};
        // checking if the token is a bang_equal operator
        else if (token_text == "!=")
            return {TokenType::BangEqual, token_text};
        // checking if the token is a greater_than operator
        else if (token_text == ">")
            return {TokenType::Greater, token_text};
        // checking if the token is a greater_equal operator
        else if (token_text == ">=")
            return {TokenType::GreaterEqual, token_text};
        // checking if the token is a less_than operator
        else if (token_text == "<")
            return {TokenType::Less, token_text};
        // checking if the token is a less_equal operator
        else if (token_text == "<=")
            return {TokenType::LessEqual, token_text};
    } else if (isalpha(token_text[0]) && token_text.contains(":")) {
        return {TokenType::Symbol, token_text};
    }

    // if the execution arrives here, it is an invalid one
    return {TokenType::Invalid, token_text};
}

void Lexer::set_content(std::string new_content) {
    this->content = std::move(new_content);
}

std::vector<Token> Lexer::collect() {
    // collecting all the tokens into a vector
    std::vector<Token> tokens = std::vector<Token>();
    Token token = Token();
    while ((token = this->next_token()).get_type() != TokenType::E_O_F) tokens.push_back(token);
    return tokens;
}
