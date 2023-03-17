//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_QUERY_H
#define JDBGE_QUERY_H

#include <string>
#include <utility>
#include <vector>

#include "Filter.h"

typedef struct Item {
    std::string column;
    std::string value;
    std::string type;
} Item;

class Query {
public:
    explicit Query(std::string opcode = "",
                 std::string target = "",
                 std::vector<Item> columns = {},
                 Filter filter = Filter(),
                 int limit = 0) :
                 opcode(std::move(opcode)),
                 target(std::move(target)),
                 columns(std::move(columns)),
                 filter(std::move(filter)),
                 limit(limit) {}

    friend std::ostream &operator<<(std::ostream &os, const Query &obj) {
        std::cout << "Query: " << std::endl;
        std::cout << "\topcode: " << obj.opcode << std::endl;
        std::cout << "\ttarget: " << obj.target << std::endl;
        std::cout << "\tlimit : " << obj.limit << std::endl;
        std::cout << "\titems : {" << std::endl;
        for (size_t i = 0; i < (obj.columns).size(); i++)
            std::cout << "\t\titem {\n"
                << "\t\t\tcolumn: " << (obj.columns)[i].column << std::endl
                << "\t\t\ttype: " << (obj.columns)[i].type << std::endl
                << "\t\t\tvalue: " << (obj.columns)[i].value << std::endl
                << "\t\t}" << std::endl;
        std::cout << "\t}" << std::endl;

        return os;
    }

private:
    std::string opcode;
    std::string target;
    std::vector<Item> columns;
    Filter filter;
    int limit;
};

//
//#define EMPTY_QUERY {"", "", {}, Filter(), 0}
//#define print_query(_QUERY) std::cout << "Query: " << std::endl; \
//    std::cout << "\topcode: " << _QUERY.opcode << std::endl; \
//    std::cout << "\ttarget: " << _QUERY.target << std::endl; \
//    std::cout << "\tlimit : " << _QUERY.limit << std::endl; \
//    std::cout << "\titems : " << std::endl; \
//    for (size_t i = 0; i < (_QUERY.columns).size(); i++) \
//            std::cout << "\t{\n" \
//                << "\t\t" << (_QUERY.columns)[i].column << ",\n" \
//                << "\t\t" << (_QUERY.columns)[i].type << ",\n" \
//                << "\t\t" << (_QUERY.columns)[i].value \
//                << std::endl; \
//        std::cout << "\t}" << std::endl;

#endif //JDBGE_QUERY_H
