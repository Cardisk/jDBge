//
// Created by Matteo Cardinaletti on 12/03/23.
//

#include "Filter.h"

Table Filter::filter_table(Table table) {
    return table;
}

void Filter::push_op(Expression expr) {
    this->ops.push_back(expr);
}

void Filter::push_port(Boolean port) {
    this->ports.push_back(port);
}
