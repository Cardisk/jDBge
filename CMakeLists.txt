cmake_minimum_required(VERSION 3.24)
project(jDBge)

set(CMAKE_CXX_STANDARD 23)

add_executable(jDBge main.cpp components/Lexer.cpp components/Token.cpp components/Token.h components/Keywords.h components/Query.h components/Filter.cpp components/Filter.h components/Table.h components/Parser.cpp components/Parser.h components/VM.cpp components/VM.h)
