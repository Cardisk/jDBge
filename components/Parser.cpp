//
// Created by Matteo Cardinaletti on 13/03/23.
//

#include "Parser.h"

#include <iostream>
#include <algorithm>

bool Parser::validate_query() {
    // if there are no tokens available, the query is not valid
    if (this->tokens.empty()) return false;
    // if the query doesn't start with a keyword, it isn't valid
    if (this->tokens[0].get_type() != TokenType::Keyword) return false;
    // if there is at least one invalid token, the query isn't valid
    if (std::all_of(this->tokens.begin(), this->tokens.end(),
                    [](const Token &t) { return t.get_type() != TokenType::Invalid; }))
        return true;

    return false;
}

/// Private function to organize the code.
/// It returns the next token inside the vector.
/// \param vector Tokens
/// \return A new Token
Token vector_pop(std::vector<Token> &vector) {
    // if there are no tokens left, return an EOF
    if (vector.empty()) return Token("");
    Token temp = vector.back();
    vector.pop_back();
    return temp;
}

/// Private function to organize the code.
/// It returns an array of substrings split by the delimiter.
/// \param str Raw string
/// \param delim Delimiter
/// \return Vector of substrings
std::vector<std::string> string_split(std::string str, const std::string &delim) {
    std::vector<std::string> v;
    size_t next_pos;
    do {
        next_pos = str.find(delim);
        std::string txt = str.substr(0, next_pos);
        str.erase(0, next_pos + delim.length());

        // removing double quotes if provided by the user
        if (txt.starts_with("\"") && txt.ends_with("\"")) {
            txt.pop_back();
            txt.erase(0, 1);
        }

        if (!txt.empty()) v.push_back(txt);
    } while (next_pos != std::string::npos);

    return v;
}

/// Private function to organize the code.
/// Pushes an item inside the given vector.
/// \param columns Where to store data
/// \param token A Token
/// \param opcode Which query require that argument
/// \return A boolean representing the result of the function
bool push_arg(std::vector<Item> &columns, Token &token, const std::string &opcode) {
    std::vector<std::string> v = string_split(token.get_text(), ":");
    Item i;
    // default for all the commands
    i.column = v[0];
    if (v.size() > 1 && (opcode == "table" || opcode == "insert")) {
        // if it is a 'table' query, store the type
        if (opcode == "table") i.type = v[1];
        // if it is a 'insert' query, store the value
        else if (opcode == "insert") i.value = v[1];
    }
    else if (opcode == "table" || opcode == "insert") {
        // if the execution reaches this point, it's an invalid token
        std::cerr << "Invalid token '" << token.get_text() << "'" << std::endl;
        return false;
    }

    columns.push_back(i);
    return true;
}

/// Private function to organize the code.
/// Recognize an expression after the filter keyword
/// \param t Tokens
/// \return A new Expression
Expression parse_expr(std::vector<Token> &t) {
    std::vector<Token> expr(3);
    // pop three tokens out of the vector
    for (int i = 0; i < 3; ++i) {
        expr.push_back(vector_pop(t));
    }

    Expression e;
    // if the first one is EOF, return en empty expression
    if (expr[0].get_type() == TokenType::E_O_F) return EMPTY_EXPR;
    else e.left = expr[0].get_text();

    // recognizing the operand
    switch (expr[1].get_type()) {
        case TokenType::Equal:
        case TokenType::BangEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
            e.op = expr[1].get_text();
            break;

        default:
            // if the execution reaches this point, is an invalid expression
            std::cerr << "Invalid expression provided inside 'filter'" << std::endl;
            return EMPTY_EXPR;
    }

    // if the third one isn't EOF, store it inside the expression
    if (expr[2].get_type() != TokenType::E_O_F) e.right = expr[2].get_text();
    else return EMPTY_EXPR;

    return e;
}

#define has_tokens !this->tokens.empty()
#define tokens_peek this->tokens.back()

Query Parser::compile_query() {
    // validating
    if (!this->validate_query()) {
        std::cerr << "Invalid query provided" << std::endl;
        return EMPTY_QUERY;
    }

    // reverse the tokens to treat them as a stack
    std::reverse(this->tokens.begin(), this->tokens.end());
    // opcode for the query command
    std::string opcode = vector_pop(this->tokens).get_text();

    Token temp = vector_pop(this->tokens);
    // number of rows that needs to be retrieved (default -1 means all)
    std::string limit = "-1";
    // target of the command
    std::string target;
    if (temp.get_type() == TokenType::Number && opcode == "select")
        limit = temp.get_text();
    else if (temp.get_type() == TokenType::Symbol)
        target = temp.get_text();
    else {
        // if the execution reaches this point, is an invalid query
        std::cerr << "Invalid token '" << temp.get_text() << "'" << std::endl;
        return EMPTY_QUERY;
    }

    // if the target is still empty store it
    if (target.empty() && has_tokens &&
        tokens_peek.get_type() == TokenType::Symbol)
            target = vector_pop(this->tokens).get_text();

    // store the arguments of the query (if no one is provided it means that the user needs all of them)
    std::vector<Item> columns;
    if (tokens_peek.get_type() != TokenType::Keyword && tokens_peek.get_type() != TokenType::E_O_F) {
        temp = vector_pop(this->tokens);
        while (temp.get_type() == TokenType::Symbol) {
            bool result = push_arg(columns, temp, opcode);
            if (!result) return EMPTY_QUERY;
            if (tokens_peek.get_type() == TokenType::Symbol) temp = vector_pop(this->tokens);
            else break;
        }
    }

    // metal detector for ugly queries
    if (opcode == "table" && columns.empty()) {
        std::cerr << "Invalid 'table' query" << std::endl;
        return EMPTY_QUERY;
    }

    // query filter
    Filter filter;
    temp = vector_pop(this->tokens);
    if (temp.get_type() == TokenType::Keyword && temp.get_text() == "filter") {
        // if there is nothing after it, it is an invalid query
        if (!has_tokens) {
            std::cerr << "Found 'filter' token but there is no expression after it" << std::endl;
            return EMPTY_QUERY;
        }

        // while there are expressions, try to parse them
        while (has_tokens && tokens_peek.get_type() != TokenType::E_O_F) {
            Expression expr = parse_expr(this->tokens);
            if (expr == EMPTY_EXPR) {
                std::cerr << "Found 'filter' token but there is no expression after it" << std::endl;
                return EMPTY_QUERY;
            }
            filter.push_op(expr);

            // if there is a boolean operand parse it
            if ((temp = vector_pop(this->tokens)).get_type() == TokenType::Keyword) {
                if (temp.get_text() == "and") filter.push_port(Boolean::And);
                else if (temp.get_text() == "or") filter.push_port(Boolean::Or);
                else {
                    std::cerr << "Invalid boolean operand provided inside 'filter'" << std::endl;
                    return EMPTY_QUERY;
                }

                // if after the boolean there is an EOF, it is an invalid query
                if (!has_tokens) {
                    std::cerr << "Found boolean operand but any expression is provided after it" << std::endl;
                    return EMPTY_QUERY;
                }
            } else if (temp.get_type() != TokenType::E_O_F){
                std::cerr << "Invalid token '" << temp.get_text() << "' found where a boolean operand is expected" << std::endl;
                return EMPTY_QUERY;
            }
        }
    }

    return Query(opcode, target, columns, filter, std::stoi(limit));
}
