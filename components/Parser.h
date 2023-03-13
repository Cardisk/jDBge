//
// Created by Matteo Cardinaletti on 13/03/23.
//

#ifndef JDBGE_PARSER_H
#define JDBGE_PARSER_H

#include <utility>
#include <vector>

#include "Token.h"
#include "Query.h"

class Parser {
public:
    explicit Parser(std::vector<Token> v_tokens) : tokens(std::move(v_tokens)) {}

    bool validate_query();
    Query compile_query();

    void set_tokens(std::vector<Token> new_tokens) {
        this->tokens = std::move(new_tokens);
    }

private:
    std::vector<Token> tokens;
};


#endif //JDBGE_PARSER_H
