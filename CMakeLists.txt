cmake_minimum_required(VERSION 3.24)
project(jDBge)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

set(HEADERS include/Token.h include/Keywords.h include/Query.h include/Filter.h include/Table.h include/Parser.h include/VM.h include/Console.h include/Logger.h)

set(SOURCES src/main.cpp src/Lexer.cpp src/Token.cpp src/Filter.cpp src/Parser.cpp src/VM.cpp)

add_executable(jDBge ${HEADERS} ${SOURCES})
