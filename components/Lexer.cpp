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

    // tokenizing string literals with spaces inside them
    if (token_text.contains("\"") && !token_text.ends_with('"')) {
        next_token_pos = this->content.find('"');
        token_text += this->content.substr(0, next_token_pos + 1);
        this->content = this->content.erase(0, next_token_pos + 1);
    }

    // tokenizing brackets
    if (token_text.starts_with('(') || token_text.starts_with(')')) {
        bool bra = token_text.starts_with('(');
        token_text.erase(0, 1);
        this->content.insert(0, token_text);

        if (bra)
            return {TokenType::Bra, "("};
        else
            return {TokenType::Ket, ")"};
    }

    if (token_text.ends_with(')')) {
        token_text.erase(token_text.size() - 1, 1);
        this->content.insert(0, ")");
    }

    // this will check only if the token can be a possible meta_cmd
    if (token_text.starts_with('.'))
        return {TokenType::Meta_cmd, token_text};

    // checking if the token is a keyword
    if (std::any_of(keywords.begin(), keywords.end(),
                    [&token_text](const std::string &keyword) { return keyword == token_text; }))
        return {TokenType::Keyword, token_text};

    // checking if the token is a number
    if (isdigit(token_text[0])) {
        if (std::all_of(token_text.begin(), token_text.end(),
                        [](const char &c) { return isalpha(c); }))
            return {TokenType::Number, token_text};
    }

    // removing leading and trailing '"'
    if (token_text.length() > 2 && token_text.starts_with('"') && token_text.ends_with('"')) {
        token_text = token_text.erase(0, 1);
        token_text = token_text.erase(token_text.length() - 1, token_text.length());

        return {TokenType::Symbol, token_text};
    }

    // checking if the token is a symbol
    if (std::all_of(token_text.begin(), token_text.end(),
                    [](const char &c) { return (isalpha(c) || c == '_'); })) {
        return {TokenType::Symbol, token_text};
    }

    // checking if the token is an is_equal operator
    if (token_text == "==" || token_text == "=")
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

    if (isalpha(token_text[0]) && token_text.contains(':'))
        return {TokenType::Symbol, token_text};

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
