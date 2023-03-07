//
// Created by Matteo Cardinaletti on 06/03/23.
//

#include "Token.h"

std::string token_to_str(TokenType type) {
    static_assert(TokenType::TYPE_COUNT == 11);

    switch (type) {
        case TokenType::E_O_F: return "EOF";
        case TokenType::Invalid: return "Invalid";
        case TokenType::Symbol: return "Symbol";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Number: return "Number";
        case TokenType::Equal: return "Equal";
        case TokenType::BangEqual: return "BangEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::TYPE_COUNT: return "TYPE_COUNT";
        default: return "unknown token type";
    }
}