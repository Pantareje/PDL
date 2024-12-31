cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ICU REQUIRED COMPONENTS uc)

add_executable(PDL
        src/Characters.h
        src/Characters.cpp
        src/Token.h
        src/LexicalError.h
        src/Lexer.h
        src/SymbolTables.h
        src/Parser.h
        src/SyntaxException.h
        src/Application.h
        src/Application.cpp
        src/Main.cpp
        src/ApplicationAttributes.h
        src/SemanticState.h
        src/Parser.cpp
        src/StringProduct.h
)

target_link_libraries(PDL PRIVATE ICU::uc)
