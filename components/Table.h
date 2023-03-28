//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_TABLE_H
#define JDBGE_TABLE_H

#include <string>
#include <map>
#include <vector>

/// Representation of a Row
typedef struct Row {
    std::vector<std::string> values;
} Row;

/// Representation of a Table
typedef struct Table {
    std::string name;
    std::map<std::string, int> schema;
    std::vector<Row> rows;
} Table;

#endif //JDBGE_TABLE_H
