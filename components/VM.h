//
// Created by leonardo on 20/03/23.
//

#ifndef JDBGE_VM_H
#define JDBGE_VM_H

#include <string>
#include <vector>
#include "Query.h"
#include "Filter.h"

class VM {
public:
    bool exec_query(Query query);
    bool get_available_dbs(Query query);
    bool get_schema(Query query);
    bool get_tables(Query query);

private:
    std::string current_db;

    bool do_select(std::string table_name, std::vector<Item> columns, Filter filter);
    bool do_insert(std::string table_name, std::vector<Item> columns);
    bool do_remove(std::string table_name, Filter filter);
    bool do_delete(std::string target);
    bool do_db(std::string db_name);
    bool do_table(std::string table_name);

};


#endif //JDBGE_VM_H
