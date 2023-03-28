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

    bool exec_query(Query const& query);
    bool get_available_dbs(Query const& query);
    bool get_schema(Query const& query);
    bool get_tables(Query const& query);

private:
    std::string current_db;
    Table databases;

    bool do_select(std::string table_name, std::vector<Item> columns, Filter filter);
    bool do_insert(std::string table_name, std::vector<Item> columns);
    bool do_remove(std::string table_name, Filter filter);
    bool do_delete(std::string target);
    bool do_db(std::string db_name);
    bool do_table(std::string table_name);

};


#endif //JDBGE_VM_H
