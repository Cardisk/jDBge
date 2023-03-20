//
// Created by Matteo Cardinaletti on 13/03/23.
//

#include "Parser.h"

#include <iostream>
#include <algorithm>

bool Parser::validate_query() {
    if (this->tokens.empty()) return false;
    if (this->tokens[0].get_type() != TokenType::Keyword) return false;
    if (std::all_of(this->tokens.begin(), this->tokens.end(),
                    [](const Token &t) { return t.get_type() != TokenType::Invalid; }))
        return true;

    return false;
}

Token vector_pop(std::vector<Token> &vector) {
    if (vector.empty()) return Token("");
    Token temp = vector.back();
    vector.pop_back();
    return temp;
}

std::vector<std::string> string_split(std::string str, const std::string &delim) {
    std::vector<std::string> v;
    size_t next_pos;
    do {
        next_pos = str.find(delim);
        std::string txt = str.substr(0, next_pos);
        str.erase(0, next_pos + delim.length());

        if (!txt.empty()) v.push_back(txt);
    } while (next_pos != std::string::npos);

    return v;
}

bool push_arg(std::vector<Item> &columns, Token &token, const std::string &opcode) {
    std::vector<std::string> v = string_split(token.get_text(), ":");
    Item i;
    i.column = v[0];
    if (v.size() > 1 && (opcode == "table" || opcode == "insert")) {
        if (opcode == "table") i.type = v[1];
        else if (opcode == "insert") i.value = v[1];
    }
    else if (opcode == "table" || opcode == "insert") {
        std::cerr << "Invalid token '" << token.get_text() << "'" << std::endl;
        return false;
    }

    columns.push_back(i);
    return true;
}

Expression parse_expr(std::vector<Token> &t) {
    std::vector<Token> expr;
    for (int i = 0; i < 3; ++i) {
        expr.push_back(vector_pop(t));
    }

    Expression e;
    if (expr[0].get_type() == TokenType::E_O_F) return EMPTY_EXPR;
    else e.left = expr[0].get_text();

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
            std::cerr << "Invalid expression provided inside 'filter'" << std::endl;
            return EMPTY_EXPR;
    }

    if (expr[2].get_type() != TokenType::E_O_F) e.right = expr[2].get_text();
    else return EMPTY_EXPR;

    return e;
}

#define has_tokens !this->tokens.empty()
#define tokens_peek this->tokens.back()

Query Parser::compile_query() {
    if (!this->validate_query()) {
        std::cerr << "Invalid query provided" << std::endl;
        return EMPTY_QUERY;
    }

    std::reverse(this->tokens.begin(), this->tokens.end());
    std::string opcode = vector_pop(this->tokens).get_text();
    Token temp = vector_pop(this->tokens);
    std::string limit = "-1";
    std::string target;
    if (temp.get_type() == TokenType::Number && opcode == "select")
        limit = temp.get_text();
    else if (temp.get_type() == TokenType::Symbol)
        target = temp.get_text();
    else {
        std::cerr << "Invalid token '" << temp.get_text() << "'" << std::endl;
        return EMPTY_QUERY;
    }

    if (target.empty() && has_tokens) target = vector_pop(this->tokens).get_text();

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

    if (opcode == "table" && columns.empty()) {
        std::cerr << "Invalid 'table' query" << std::endl;
        return EMPTY_QUERY;
    }

    Filter filter;
    temp = vector_pop(this->tokens);
    if (temp.get_type() == TokenType::Keyword && temp.get_text() == "filter") {
        while (has_tokens && tokens_peek.get_type() != TokenType::E_O_F) {
            Expression expr = parse_expr(this->tokens);
            if (expr.left.empty() || expr.op.empty() || expr.right.empty()) {
                std::cerr << "Found 'filter' token but there is no expression after it";
                return EMPTY_QUERY;
            }
            filter.push_op(expr);

            std::cout << expr << std::endl;

            if ((temp = vector_pop(this->tokens)).get_type() == TokenType::Keyword) {
                std::cout << temp << std::endl;
                if (temp.get_text() == "and") filter.push_port(Boolean::And);
                else if (temp.get_text() == "or") filter.push_port(Boolean::Or);
                else {
                    std::cerr << "Invalid boolean operand provided inside 'filter'" << std::endl;
                    return EMPTY_QUERY;
                }

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
