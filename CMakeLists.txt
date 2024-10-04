cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PDL main.cpp
        Lexer.h
        Token.h
        LexicalException.h
        Characters.h
        Characters.cpp)
