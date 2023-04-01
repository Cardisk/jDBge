//
// Created by leonardo on 20/03/23.
//

#include "VM.h"
#include <algorithm>

VM::VM() {
    current_db = "";
}

bool VM::exec_query(Query const &query) {
    std::string opcode = query.getOpcode();

    if (opcode == "table") {
        this->do_table(query.getTarget(), query.getColumns());
    } else if (opcode == "db") {
        this->do_db(query.getTarget());
    } else if (opcode == "select") {
        this->do_select(query.getTarget(), query.getColumns(), query.getFilter());
    } else if (opcode == "insert") {
        this->do_insert(query.getTarget(), query.getColumns());
    } else if (opcode == "remove") {
        this->do_remove(query.getTarget(), query.getFilter());
    } else if (opcode == "delete") {
        this->do_delete(query.getTarget());
    } else if (opcode == "") {
        return false;
    }
    return true;
}

void VM::flush() {
    query_result.rows.clear();
    query_result.schema.clear();
}

std::map<std::string, Column> to_map(std::vector<Item> const &columns) {
    int index = 0;
    std::map<std::string, Column> schema;
    for (Item item: columns) {
        schema.insert({item.column, Column{index++, item.type}});
    }
    return schema;
}

// TODO: Do filter on query result, was not implemented due to Filter not being ready
bool VM::do_select(std::string const &table_name, std::vector<Item> const &fields, Filter const &filter) {
    std::vector<Table> const &database = databases[current_db];
    for (Table const &table: database) {
        if (table.name == table_name) {
            flush();
            query_result.schema = to_map(fields);
            query_result.rows = table.rows;
            return true;
        }
    }
    return false;
}


bool VM::do_insert(std::string const &table_name, const std::vector<Item> &fields) {
    std::vector<Table> const &database = databases[current_db];
    for (Table table: database) {
        if (table.name == table_name) {
            Row new_row;
            for (Item const &field: fields) {
                new_row.values.insert(new_row.values.begin() + table.schema.at(field.column).index, field.value);
            }
            table.rows.push_back(new_row);
            return true;
        }
    }
    return false;
}

bool VM::do_remove(std::string const &table_name, Filter filter) {
    std::vector<Table> &database = databases[current_db];
    for (auto it = database.begin(); it != database.end(); it++) {
        if ((*it).name == table_name) {
            database.erase(it);
            return true;
        }
    }
    return false;
}

bool VM::do_delete(std::string const &target) {
    databases.erase(target);
    return true;
}

bool VM::do_db(const std::string &db_name) {
    databases.insert({db_name, std::vector<Table>()});
    return true;
}

bool VM::do_table(const std::string &table_name, std::vector<Item> schema) {
    std::vector<Table> &database = databases[current_db];
    Table new_table;
    new_table.schema = to_map(schema);
    database.push_back(new_table);
    return true;
}