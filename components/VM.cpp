//
// Created by leonardo on 20/03/23.
//

#include "VM.h"

bool VM::exec_query(Query const& query) {
    std::string opcode = query.getOpcode();

    if (opcode == 'select')
}