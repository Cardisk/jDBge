//
// Created by Matteo Cardinaletti on 06/03/23.
//

#ifndef JDBGE_TOKEN_H
#define JDBGE_TOKEN_H

#include <string>

typedef enum TokenType {
    Symbol,
    Keyword,
    BinaryOp,
    UnaryOp,
    Number,
    TYPE_COUNT,
} TokenType;

typedef struct Token {
    TokenType token_type;
    std::string token_text;
} Token;

#endif //JDBGE_TOKEN_H
