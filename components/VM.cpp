//
// Created by leonardo on 20/03/23.
//

#include "VM.h"

#include <algorithm>
#include <iostream>

#include "Logger.h"

VM::VM() {
    current_db = "";
}

bool VM::exec_query(Query const &query) {
    const std::string &opcode = query.getOpcode();

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
    } else if (opcode.empty()) {
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
    for (const Item &item: columns) {
        schema.insert({item.column, Column{index++, item.type}});
    }
    return schema;
}

// TODO: Do filter on query result, was not implemented due to Filter not being ready
bool VM::do_select(std::string const &table_name, std::vector<Item> const &fields, Filter const &filter) {
    Logger &logger = Logger::get_instance();

    if (current_db.empty()) {
        logger.error("no database selected");
        return false;
    }

    std::vector<Table> const &database = databases.at(current_db);
    for (Table const &table: database) {
        if (table.name == table_name) {
            flush();
            query_result.name = table_name;
            if (fields.empty()) {
                query_result.schema = table.schema;
            } else {
                query_result.schema = to_map(fields);
            }
            query_result.rows = table.rows;

            std::cout << query_result.name << std::endl;
            for (const auto& column: query_result.schema)
                std::cout << column.first << '\t' << '\t';
            std::cout << std::endl;

            for (const Row& row: query_result.rows) {
                for (const std::string& value: row.values)
                    std::cout << value << '\t' << '\t';
                std::cout << std::endl;
            }

            return true;
        }
    }
    logger.error("'" + table_name + "' does not exists");
    return false;
}

bool VM::do_select(const std::string &table_name, const std::vector<Item> &fields) {
    return do_select(table_name, fields, Filter());
}

bool VM::do_insert(std::string const &table_name, const std::vector<Item> &fields) {
    Logger &logger = Logger::get_instance();
    std::vector<Table> &database = databases.at(current_db);
    for (Table &table: database) {
        if (table.name == table_name) {
            Row new_row;
            for (Item const &field: fields) {
                if (table.schema.find(field.column) == table.schema.end()) {
                    logger.error("'" + field.column + "' does not exist inside '" + table_name + "' table");
                    return false;
                }

                new_row.values.insert(new_row.values.begin() + table.schema.at(field.column).index, field.value);
            }
            table.rows.push_back(new_row);
            return true;
        }
    }
    logger.error("'" + table_name + "' does not exists");
    return false;
}

bool VM::do_remove(std::string const &table_name, Filter filter) {
    Logger &logger = Logger::get_instance();
    std::vector<Table> &database = databases.at(current_db);
    for (auto it = database.begin(); it != database.end(); it++) {
        if ((*it).name == table_name) {
            database.erase(it);
            return true;
        }
    }
    logger.error("'" + table_name + "' does not exists");
    return false;
}

bool VM::do_delete(std::string const &target) {
    databases.erase(target);
    return true;
}

bool VM::do_db(const std::string &db_name) {
    if (databases.contains(db_name))
        current_db = db_name;
    else {
        databases.insert({db_name, std::vector<Table>()});
        current_db = db_name;
    }
    return true;
}

void VM::do_table(const std::string &table_name, const std::vector<Item> &schema) {
    std::vector<Table> &database = databases.at(current_db);
    for (const Table &table: database) {
        if (table.name == table_name) {
            return;
        }
    }
    Table new_table;
    new_table.name = table_name;
    new_table.schema = to_map(schema);
    database.push_back(new_table);
}

bool VM::do_remove(const std::string &table_name) {
    return do_remove(table_name, Filter());
}

std::vector<std::string> VM::get_available_dbs() {
    std::vector<std::string> dbs;
    for (auto it = databases.begin(); it != databases.end(); ++it) {
        dbs.push_back(it->first);
    }
    return dbs;
}

Table VM::get_schema(const std::string &db_name, const std::string &table_name) {
    Table schema = Table();
    for (const Table &table: databases[db_name]) {
        if (table.name == table_name) {
            schema.schema = table.schema;
        }
    }
    return schema;
}

std::vector<std::string> VM::get_tables_names(const std::string &db_name) {
    std::vector<std::string> tables_names;
    for (const Table &table: databases[db_name]) {
        tables_names.push_back(table.name);
    }
    return tables_names;
}

