//
// Created by Matteo Cardinaletti on 06/03/23.
//

#ifndef JDBGE_TOKEN_H
#define JDBGE_TOKEN_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

/// Enumeration representing the type of the token.
typedef enum TokenType {
    E_O_F = 0,
    Meta_cmd,
    Invalid,

    Symbol,
    Keyword,
    Number,

    Equal,
    BangEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    Bra,
    Ket,

    TYPE_COUNT,
} TokenType;

/// Converting a TokenType into a string.
std::string token_to_str(TokenType type);

/// Implementation of a token as a class.
class Token {
public:
    Token(TokenType tokenType, std::string tokenText) : token_type(tokenType), token_text(std::move(tokenText)) {}

    explicit Token(std::string tokenText = "") : token_type(TokenType::E_O_F), token_text(std::move(tokenText)) {}

    friend std::ostream &operator<<(std::ostream &os, const Token &obj) {
        os << "{ ";
        os << "token_type: \'" << token_to_str(obj.token_type) << "\'";
        os << ", token_text: \'" << obj.token_text << "\'";
        os << " }";
        return os;
    };

    friend bool operator==(const Token &self, const Token &other) {
        return (self.token_type == other.token_type && self.token_text == other.token_text);
    }

    [[nodiscard]] TokenType get_type() const {
        return token_type;
    }

    [[nodiscard]] const std::string &get_text() const {
        return token_text;
    }

private:
    TokenType token_type;
    std::string token_text;
};

#endif //JDBGE_TOKEN_H
