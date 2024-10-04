cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ICU REQUIRED COMPONENTS uc)

add_executable(PDL
        src/main.cpp
        src/Lexer.h
        src/Token.h
        src/LexicalException.h
        src/Characters.h
        src/Characters.cpp
        src/SymbolTable.h)

target_link_libraries(PDL PRIVATE ICU::uc)
