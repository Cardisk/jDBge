//
// Created by leonardo on 20/03/23.
//

#include "VM.h"

VM::VM() {
    Table database_schema;
    std::map<std::string, int> schema;
    schema.insert({"db_name", 0});
    database_schema.name = ".schema";
    database_schema.schema = schema;
    this->database.push_back(database_schema);
}

bool VM::exec_query(Query const& query) {
    std::string opcode = query.getOpcode();

    if (opcode == "table"){
        this->do_table(query.getTarget(), query.getColumns());
    } else if (opcode == "db"){
        this->do_db(query.getTarget());
    } else if (opcode == "select"){
        this->do_select(query.getTarget(), query.getColumns(), query.getFilter());
    } else if (opcode == "insert"){
        this->do_insert(query.getTarget(), query.getColumns());
    } else if (opcode == "remove"){
        this->do_remove(query.getTarget(), query.getFilter());
    } else if (opcode == "delete"){
        this->do_delete(query.getTarget());
    }
}

std::vector<std::string> VM::get_available_dbs() {
    std::vector<std::string> = {Item};
    this->do_select(".schema", Item("db_name", "", "string"));
}
