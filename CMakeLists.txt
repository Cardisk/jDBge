cmake_minimum_required(VERSION 3.24)
project(jDBge)

set(CMAKE_CXX_STANDARD 23)

add_executable(jDBge main.cpp Lexer.cpp Lexer.h)
