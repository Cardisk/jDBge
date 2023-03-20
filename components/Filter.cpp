//
// Created by Matteo Cardinaletti on 12/03/23.
//

#include "Filter.h"

Table Filter::filter_table(Table table) {
    return table;
}

std::string boolean_to_str(const Boolean b) {
    switch (b) {
        case Boolean::And: return "and";
        case Boolean::Or: return "or";
    }
}

void Filter::push_op(Expression expr) {
    this->ops.push_back(expr);
}

void Filter::push_port(Boolean port) {
    this->ports.push_back(port);
}
