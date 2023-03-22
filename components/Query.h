//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_QUERY_H
#define JDBGE_QUERY_H

#include <string>
#include <utility>
#include <vector>
#include <ostream>

#include "Filter.h"

/// Representing an argument for a generic query.
class Item {
public:
    explicit Item(std::string column = "",
                  std::string value = "",
                  std::string type = "") :
            column(std::move(column)),
            value(std::move(value)),
            type(std::move(type)) {}

    friend std::ostream &operator<<(std::ostream &os, const Item &item) {
        os << "{ column: '" << item.column << "', value: '" << item.value << "', type: '" << item.type << "' }";
        return os;
    }

    friend bool operator==(const Item &self, const Item &other) {
        return (self.column == other.column && self.value == other.value && self.type == other.type);
    }

    std::string column;
    std::string value;
    std::string type;
};

/// Implementation of a query as a class.
class Query {
public:
    explicit Query(std::string opcode = "",
                   std::string target = "",
                   std::vector<Item> columns = {},
                   Filter filter = Filter(),
                   int limit = -1) :
            opcode(std::move(opcode)),
            target(std::move(target)),
            columns(std::move(columns)),
            filter(std::move(filter)),
            limit(limit) {}

    friend std::ostream &operator<<(std::ostream &os, const Query &obj) {
        os << "Query: " << std::endl;
        os << "\topcode: " << obj.opcode << std::endl;
        os << "\ttarget: " << obj.target << std::endl;
        os << "\tlimit : " << obj.limit << std::endl;
        os << "\titems : {" << std::endl;
        for (const auto &column: obj.columns)
            os << "\t\t" << column << std::endl;
        os << "\t}" << std::endl;
        os << "\tfilter: { " << obj.filter << " }";
        return os;
    }

    friend bool operator==(const Query &self, const Query &other) {
        if (self.opcode != other.opcode) return false;

        return (self.target == other.target && self.columns == other.columns &&
                self.filter == other.filter && self.limit == other.limit);
    }

    friend bool operator!=(const Query &self, const Query &other) {
        return !operator==(self, other);
    }

private:
    std::string opcode;
    std::string target;
    std::vector<Item> columns;
    Filter filter;
    int limit;
};

#define EMPTY_QUERY Query()

#endif //JDBGE_QUERY_H
