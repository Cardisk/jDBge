//
// Created by Matteo Cardinaletti on 13/03/23.
//

#ifndef JDBGE_PARSER_H
#define JDBGE_PARSER_H

#include <utility>
#include <vector>

#include "Token.h"
#include "Query.h"

/// Implementation of a parser.
/// It is capable of validating and compiling queries.
class Parser {
public:
    explicit Parser(std::vector<Token> v_tokens = {}) : tokens(std::move(v_tokens)) {}

    /// Validating a query.
    bool validate_query();

    /// Compiling a query.
    Query compile_query();

    std::vector<Query> parse_tokens();

    /// Setting new tokens to the parser.
    void set_tokens(std::vector<Token> new_tokens) {
        this->tokens = std::move(new_tokens);
    }

private:
    bool query(std::vector<Query> &queries);

    std::vector<Token> tokens;
};


#endif //JDBGE_PARSER_H
