//
// Created by leonardo on 20/03/23.
//

#ifndef JDBGE_VM_H
#define JDBGE_VM_H

#include <string>
#include <vector>
#include "Query.h"
#include "Filter.h"
#include "Table.h"

class VM {
public:
    Table query_result;

    explicit VM();

    bool exec_query(Query const &query);

    std::vector<std::string> get_available_dbs();

    bool get_schema(Query const &query);

    bool get_tables(Query const &query);

private:
    std::string current_db;
    std::map<std::string, std::vector<Table>> databases;//double map?

    void flush();

    bool do_select(std::string const &table_name, std::vector<Item> const &fields, Filter const &filter);

    bool do_insert(std::string const &table_name, std::vector<Item> const &fields);

    bool do_remove(std::string const &table_name, Filter filter);

    bool do_delete(std::string const &target);

    bool do_db(std::string const &db_name);

    void do_table(const std::string &table_name, const std::vector<Item>& schema);
};


#endif //JDBGE_VM_H
