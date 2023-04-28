//
// Created by Matteo Cardinaletti on 13/03/23.
//

#include "../include/Parser.h"
#include "../include/Logger.h"

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
    Logger &logger = Logger::get_instance();
    std::vector<std::string> v = string_split(token.get_text(), ":");
    Item i;
    // default for all the commands
    i.column = v[0];
    if (v.size() > 1 && (opcode == "table" || opcode == "insert")) {
        // if it is a 'table' query, store the type
        if (opcode == "table") i.type = v[1];
            // if it is a 'insert' query, store the value
        else if (opcode == "insert") i.value = v[1];
    } else if (opcode == "table" || opcode == "insert") {
        // if the execution reaches this point, it's an invalid token
        logger.error("invalid token '" + token.get_text() + "'");
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
    Logger &logger = Logger::get_instance();
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
            logger.error("Invalid expression provided inside 'filter'");
            return EMPTY_EXPR;
    }

    // if the third one isn't EOF, store it inside the expression
    if (expr[2].get_type() != TokenType::E_O_F) e.right = expr[2].get_text();
    else return EMPTY_EXPR;

    return e;
}

#define has_tokens !this->tokens.empty()
#define tokens_peek this->tokens.back()

bool parse_filter(std::vector<Token> &tokens, Filter &filter) {
    Logger &logger = Logger::get_instance();
    Token temp = vector_pop(tokens);
    if (temp.get_type() == TokenType::Keyword && temp.get_text() == "filter") {
        // if there is nothing after it, it is an invalid query
        if (!tokens.empty()) {
            logger.error("Found 'filter' token but there is no expression after it");
            return false;
        }

        // while there are expressions, try to parse them
        while (tokens.empty() && tokens.back().get_type() != TokenType::E_O_F) {
            Expression expr = parse_expr(tokens);
            if (expr == EMPTY_EXPR) {
                logger.error("Found 'filter' token but there is no expression after it");
                return false;
            }
            filter.push_op(expr);

            // if there is a boolean operand parse it
            if ((temp = vector_pop(tokens)).get_type() == TokenType::Keyword) {
                if (temp.get_text() == "and") filter.push_port(Boolean::And);
                else if (temp.get_text() == "or") filter.push_port(Boolean::Or);
                else {
                    logger.error("Invalid boolean operand provided inside 'filter'");
                    return false;
                }

                // if after the boolean there is an EOF, it is an invalid query
                if (!tokens.empty()) {
                    logger.error("Found boolean operand but any expression is provided after it");
                    return false;
                }
            } else if (temp.get_type() != TokenType::E_O_F) {
                logger.error("invalid token '" + temp.get_text() + "' found where a boolean operand is expected");
                return false;
            }
        }
    }

    return true;
}

#define vector_print(_VECTOR) \
    do { \
        std::cout << "[ "; \
        for (size_t i = 0; i < (_VECTOR).size(); i++) \
            std::cout << (_VECTOR)[i] << (i < (_VECTOR).size() - 1 ? ", " : ""); \
        std::cout << " ]" << std::endl; \
    } while(0)

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
bool Parser::query(std::vector<Query> &queries) {
    Logger &logger = Logger::get_instance();

    // opcode for the query command
    std::string opcode = vector_pop(this->tokens).get_text();

    Token temp = vector_pop(this->tokens);
    std::string limit = "-1";
    std::string target;

    if (temp.get_type() == TokenType::Number)
        limit = temp.get_text();
    else if (temp.get_type() == TokenType::Symbol)
        target = temp.get_text();
    else {
        // if the execution reaches this point, is an invalid query
        logger.error("invalid token '" + temp.get_text() + "'");
        return false;
    }

    if (target.empty() && has_tokens && tokens_peek.get_type() == TokenType::Symbol)
        target = vector_pop(this->tokens).get_text();

    std::vector<Item> columns;
    temp = vector_pop(this->tokens);
    while (temp.get_type() == TokenType::Symbol) {
        if (!push_arg(columns, temp, opcode)) return {};

        if (!has_tokens) break;
        temp = vector_pop(this->tokens);
    }

    // metal detector for ugly queries
    if (opcode == "table" && columns.empty()) {
        logger.error("invalid 'table' query");
        return false;
    }

    Filter filter;
    while (temp.get_type() == TokenType::Keyword) {
        if (temp.get_text() == "join") {
            temp = vector_pop(this->tokens);
            if (temp.get_type() != TokenType::Bracket) {
                logger.error("Invalid token '" + temp.get_text() + "'");
                return false;
            }

            bool bracket = false;
            for (auto i = this->tokens.begin(); i < this->tokens.end(); ++i) {
                if ((*i).get_type() == TokenType::Bracket) {
                    this->tokens.erase(i);
                    bracket = true;
                    break;
                }
            }

            if (!bracket) {
                logger.error("unclosed bracket");
                return false;
            }

            if (!this->query(queries)) return false;

        } else if (temp.get_text() == "filter") {
            if (!parse_filter(this->tokens, filter)) return false;
        } else {
            // if the execution reaches this point, is an invalid query
            logger.error("invalid token '" + temp.get_text() + "'");
            return {};
        }

        if (!has_tokens) break;
        temp = vector_pop(this->tokens);
    }

    queries.emplace_back(opcode, target, columns, filter, std::stoi(limit));
    return true;
}
#pragma clang diagnostic pop

std::vector<Query> Parser::parse_tokens() {
    Logger &logger = Logger::get_instance();

    std::vector<Query> queries;

    // validating
    if (!this->validate_query()) {
        logger.error("invalid query provided");
        return {};
    }
    // reverse the tokens to treat them as a stack
    std::reverse(this->tokens.begin(), this->tokens.end());

    if (!this->query(queries)) {
        logger.error("invalid query provided");
        return {};
    }
    return queries;
}