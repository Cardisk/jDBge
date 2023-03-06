//
// Created by Matteo Cardinaletti on 06/03/23.
//

#include "Token.h"

std::string token_to_str(TokenType type) {
    static_assert(TokenType::TYPE_COUNT == 6);

    switch (type) {
        case TokenType::Invalid: return "Invalid";
        case TokenType::Symbol: return "Symbol";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Number: return "Number";
        case TokenType::BinaryOp: return "BinaryOp";
        case TokenType::UnaryOp: return "UnaryOp";
        case TokenType::TYPE_COUNT: return "TYPE_COUNT";
        default: return "unknown token type";
    }
}