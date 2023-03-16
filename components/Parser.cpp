//
// Created by Matteo Cardinaletti on 13/03/23.
//

#include "Parser.h"

#include <stack>
#include <algorithm>

bool Parser::validate_query() {
    if (this->tokens[0].get_type() != TokenType::Keyword) return false;
    if (std::all_of(this->tokens.begin(), this->tokens.end(),
                [](const Token& t) { return t.get_type() != TokenType::Invalid; }))
        return true;

    return false;
}

Token vector_pop(std::vector<Token>& vector) {
    if (vector.empty()) return Token("");
    Token temp = vector.back();
    vector.pop_back();
    return temp;
}

std::vector<std::string> string_split(std::string str, const std::string& delim) {
    std::vector<std::string> v;
    while (!str.empty()) {
        size_t next_pos = str.find(delim);
        std::string text = str.substr(0, next_pos);
        str = str.erase(0, next_pos);

        v.push_back(text);
    }

    return v;
}

void push_arg(std::vector<Item>& columns, Token& token, const std::string& opcode) {
    std::vector<std::string> v = string_split(token.get_text(), ":");
    Item i = {
            .column = "",
            .value = "",
            .type = "",
    };
    i.column = v[0];
    if (opcode == "table") i.type = v[1];
    if (opcode == "insert") i.value = v[1];

    columns.push_back(i);
}

Expression parse_expr(std::vector<Token>& t) {
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
            e.op = expr[1].get_text(); break;

        default:
            std::cerr << "Invalid expression provided inside 'filter'" << std::endl;
            return EMPTY_EXPR;
    }

    if (expr[2].get_type() != TokenType::E_O_F) e.right = expr[2].get_text();

    return e;
}

Query Parser::compile_query() {
    if (!this->validate_query()) return EMPTY_QUERY;

    std::reverse(this->tokens.begin(), this->tokens.end());
    std::string opcode = vector_pop(this->tokens).get_text();
    std::string target = vector_pop(this->tokens).get_text();

    std::vector<Item> columns;

    std::string limit = "-1";
    Token temp = vector_pop(this->tokens);
    if (temp.get_type() == TokenType::Number)
        limit = temp.get_text();
    else
        if (temp.get_type() != TokenType::E_O_F && opcode == "table" || opcode == "insert")
            push_arg(columns, temp, opcode);

    while ((temp = vector_pop(this->tokens)).get_type() == TokenType::Symbol)
        push_arg(columns, temp, opcode);

    Filter filter;
    temp = vector_pop(this->tokens);
    if(temp.get_type() == TokenType::Keyword && temp.get_text() == "filter") {
        while (!this->tokens.empty() && this->tokens.back().get_type() != TokenType::E_O_F) {
            Expression expr1 = parse_expr(this->tokens);
            if (expr1.left.empty() && expr1.op.empty() && expr1.right.empty()) {
                std::cerr << "Found 'filter' token but there is no expression after it";
                return EMPTY_QUERY;
            }
            filter.push_op(expr1);

            if((temp = vector_pop(this->tokens)).get_type() == TokenType::Keyword) {
                if (temp.get_text() == "and") filter.push_port(Boolean::And);
                else if (temp.get_text() == "or") filter.push_port(Boolean::Or);
                else {
                    std::cerr << "Invalid boolean operand provided inside 'filter'" << std::endl;
                    return EMPTY_QUERY;
                }

                Expression expr2 = parse_expr(this->tokens);
                if (expr2.left.empty() && expr2.op.empty() && expr2.right.empty()) {
                    std::cerr << "Found boolean operand but there is no expression after it";
                    return EMPTY_QUERY;
                }
                filter.push_op(expr2);
            }
        }
    }

    return Query {
        .opcode = opcode,
        .target = target,
        .columns = columns,
        .filter = filter,
        .limit = std::stoi(limit),
    };
}
