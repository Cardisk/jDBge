//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_FILTER_H
#define JDBGE_FILTER_H

#include <string>
#include <utility>
#include <vector>
#include <ostream>

#include "Table.h"

typedef enum Boolean {
    And = 0,
    Or,
} Boolean;

std::string bool_to_str(const Boolean b);

class Expression {
public:
    explicit Expression(std::string left = "",
                        std::string op = "",
                        std::string right = "") :
            left(std::move(left)),
            op(std::move(op)),
            right(std::move(right)) {}

    friend bool operator==(const Expression &self, const Expression &other) {
        return (self.left == other.left && self.op == other.op && self.right == other.right);
    }

    friend std::ostream &operator<<(std::ostream &os, const Expression &expression) {
        os << "{ left: '" << expression.left << "', op: '" << expression.op << "', right: '" << expression.right
           << "' }";
        return os;
    }

    std::string left;
    std::string op;
    std::string right;
};

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

    friend bool operator==(const Filter &self, const Filter &other) {
        return (self.ops == other.ops && self.ports == other.ports);
    }

    friend std::ostream &operator<<(std::ostream &os, const Filter &f) {
        os << "ops: { ";
        for (int i = 0; i < f.ops.size(); ++i) {
            os << f.ops[i] << (i < f.ops.size() - 1 ? ", " : "");
        }
        os << " }, ports: { ";
        for (int i = 0; i < f.ports.size(); ++i) {
            os << f.ports[i] << (i < f.ports.size() - 1 ? ", " : "");
        }
        os << " }";

        return os;
    }

private:
    std::vector<Expression> ops;
    std::vector<Boolean> ports;
};

#endif //JDBGE_FILTER_H
