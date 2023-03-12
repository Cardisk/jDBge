//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_QUERY_H
#define JDBGE_QUERY_H

#include <string>
#include <vector>

#include "Filter.h"

typedef struct Item {
    std::string column;
    std::string value;
    std::string type;
} Item;

typedef struct Query {
    std::string opcode;
    std::string target;
    std::vector<Item> columns;
    Filter filter;
    int limit;
} Query;


#endif //JDBGE_QUERY_H
