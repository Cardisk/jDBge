//
// Created by Matteo Cardinaletti on 12/03/23.
//

#ifndef JDBGE_TABLE_H
#define JDBGE_TABLE_H

#include <string>
#include <map>
#include <vector>

typedef struct Column {
    int index;
    std::string type; // might need better definition once filter is in place
} Column;

/// Representation of a Row
typedef struct Row {
    std::vector<std::string> values;
} Row;

/// Representation of a Table
typedef struct Table {
    std::string name;
    std::map<std::string, Column> schema;
    std::vector<Row> rows;
} Table;

#endif //JDBGE_TABLE_H
