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
    expr.clear();
    for (int i = 0; i < 3; ++i) {
        if (t.back().get_type() == TokenType::Ket) {
            logger.error("invalid expression provided inside 'filter'");
            return EMPTY_EXPR;
        }

        expr.push_back(vector_pop(t));
    }

    Expression e;
    // if the first one is EOF, return en empty expression
    if (expr[0].get_type() == TokenType::E_O_F) {
        logger.error("invalid expression provided inside 'filter'");
        return EMPTY_EXPR;
    }
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
    if (expr[2].get_type() != TokenType::E_O_F
            && (expr[2].get_type() == TokenType::Symbol
                || expr[2].get_type() == TokenType::Number)) e.right = expr[2].get_text();
    else {
        logger.error("invalid token '" + expr[2].get_text() + "'");
        return EMPTY_EXPR;
    }

    return e;
}

#define has_tokens !this->tokens.empty()
#define tokens_peek this->tokens.back()

bool parse_filter(std::vector<Token> &tokens, Filter &filter) {
    Logger &logger = Logger::get_instance();
    Token temp;

    while (tokens.size() >= 3 && tokens.back().get_type() != TokenType::Ket) {
        Expression expr = parse_expr(tokens);
        if (expr == EMPTY_EXPR) {
            logger.error("Found 'filter' token but there is no expression after it");
            return false;
        }
        filter.push_op(expr);

        if (!tokens.empty() && tokens.back().get_type() == TokenType::Keyword) {
            temp = vector_pop(tokens);
            if (temp.get_text() == "and") filter.push_port(Boolean::And);
            else if (temp.get_text() == "or") filter.push_port(Boolean::Or);
            else {
                logger.error("Invalid boolean operand provided inside 'filter'");
                return false;
            }

            if (tokens.empty()) {
                logger.error("Found boolean operand but any expression is provided after it");
                return false;
            }
        } else break;
    }

    if (filter.empty()) {
        logger.error("Found 'filter' token but there is no expression after it");
        return false;
    }

    return true;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
bool Parser::query(std::vector<Query> &queries) {
    Logger &logger = Logger::get_instance();

    Query query;

    Token temp = vector_pop(this->tokens);

    if (temp.get_type() == TokenType::Keyword
        && temp.get_text() != "and" && temp.get_text() != "or")
        query.setOpcode(temp.get_text());
    else {
        logger.error("invalid command '" + temp.get_text() + "'");
        return false;
    }

    temp = vector_pop(this->tokens);

    switch (temp.get_type()) {
        case TokenType::Number:
            query.setLimit(temp.get_text());
            break;
        case TokenType::Symbol:
            query.setTarget(temp.get_text());
            break;
        default:
            logger.error("invalid token '" + temp.get_text() + "'");
            return false;
    }

    if (query.getTarget().empty() && has_tokens && tokens_peek.get_type() == TokenType::Symbol)
        query.setTarget(vector_pop(this->tokens).get_text());

    do {
        if (has_tokens && tokens_peek.get_type() == TokenType::Symbol)
            temp = vector_pop(this->tokens);
        else break;

        if (!push_arg(query.getColumns(), temp, query.getOpcode())) return false;
    } while (tokens_peek.get_type() == TokenType::Symbol);

    if (query.getOpcode() == "table" && query.getColumns().empty()) {
        logger.error("invalid 'table' query");
        return false;
    }

    if (has_tokens && tokens_peek.get_type() == TokenType::Ket)
        temp = Token();
    else temp = vector_pop(this->tokens);

    if (temp.get_type() != TokenType::Keyword && temp.get_type() != TokenType::E_O_F) {
        logger.error("invalid token, found '" + temp.get_text() + "' instead of 'filter' or 'join'");
        return false;
    }

    while (temp.get_type() == TokenType::Keyword) {
        if (temp.get_text() == "join") {
            if (has_tokens && tokens_peek.get_type() == TokenType::Bra)
                vector_pop(this->tokens);
            else {
                logger.error("invalid token, found '" + this->tokens.back().get_text() + "' instead of '('");
                return false;
            }

            this->context++;
            if (!this->query(queries)) return false;
            if (has_tokens && tokens_peek.get_type() == TokenType::Ket) {
                vector_pop(this->tokens);
                this->context--;
            } else {
                logger.error("missing closed bracket");
                return false;
            }
        } else if (temp.get_text() == "filter") {
            if (!parse_filter(this->tokens, query.getFilter())) return false;
            break;
        } else {
            logger.error("invalid token, found '" + temp.get_text() + "' instead of 'filter' or 'join'");
            return false;
        }

        if (!has_tokens) break;
        if (tokens_peek.get_type() != TokenType::Ket)
            temp = vector_pop(this->tokens);
        else break;
    }

    queries.push_back(query);
    return true;
}
#pragma clang diagnostic pop

std::vector<Query> Parser::parse_tokens() {
    Logger &logger = Logger::get_instance();

    this->context = 0;
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

    if (this->context != 0 && this->tokens.empty() && !queries.empty()) {
        if (this->context > 0) logger.error("missing parenthesis");
        else logger.debug("too many parenthesis");
        return {};
    }

    return queries;
}