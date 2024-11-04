cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ICU REQUIRED COMPONENTS uc)

add_executable(PDL
        src/Main.cpp
        src/Characters.h
        src/Characters.cpp
        src/Token.h
        src/LexicalException.h
        src/Lexer.h
        src/SymbolTable.h
        src/Parser.h
        src/SyntaxException.h)

target_link_libraries(PDL PRIVATE ICU::uc)
