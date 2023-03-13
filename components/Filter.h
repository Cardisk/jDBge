//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_FILTER_H
#define JDBGE_FILTER_H

#include <string>
#include <vector>

#include "Table.h"

typedef enum Boolean {
    And = 0,
    Or,
} Boolean;

typedef struct Expression {
    std::string left;
    std::string op;
    std::string right;
} Expression;

#define EMPTY_EXPR {"", "", ""}

class Filter {
public:
    explicit Filter() : ops(), ports() {}

    void push_op(Expression expr);
    void push_port(Boolean port);

    std::vector<Expression> get_ops() {
        return ops;
    }

    std::vector<Boolean> get_ports() {
        return ports;
    }

    Table filter_table(Table table);

private:
    std::vector<Expression> ops;
    std::vector<Boolean> ports;
};

#endif //JDBGE_FILTER_H
