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

#define EMPTY_QUERY {"", "", {}, Filter(), 0}
#define print_query(_QUERY) std::cout << "Query: " << std::endl; \
    std::cout << "\topcode: " << _QUERY.opcode << std::endl; \
    std::cout << "\ttarget: " << _QUERY.target << std::endl; \
    std::cout << "\tlimit : " << _QUERY.limit << std::endl; \
    std::cout << "\titems : " << std::endl; \
    for (size_t i = 0; i < (_QUERY.columns).size(); i++) \
            std::cout << "\t{\n" \
                << "\t\t" << (_QUERY.columns)[i].column << ",\n" \
                << "\t\t" << (_QUERY.columns)[i].type << ",\n" \
                << "\t\t" << (_QUERY.columns)[i].value \
                << std::endl; \
        std::cout << "\t}" << std::endl;

#endif //JDBGE_QUERY_H
