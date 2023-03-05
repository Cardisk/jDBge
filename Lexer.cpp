//
// Created by Matteo Cardinaletti on 04/03/23.
//

#include "Lexer.h"

std::string Lexer::next_token() {
    this->trim_left();
    if (this->content.empty()) return "";

    size_t next_token_pos = this->content.find(' ');
    std::string token = this->content.substr(0, next_token_pos);
    this->content = this->content.erase(0, next_token_pos);
    return token;
}
