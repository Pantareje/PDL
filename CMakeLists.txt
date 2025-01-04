cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ICU REQUIRED COMPONENTS uc)

add_executable(PDL
        src/Characters.h
        src/Characters.cpp
        src/Token.h
        src/Lexer.h
        src/SymbolTable.h
        src/Parser.h
        src/SyntaxException.h
        src/Application.h
        src/Application.cpp
        src/Main.cpp
        src/ApplicationAttributes.h
        src/GlobalState.h
        src/Parser.cpp
        src/ValueProduct.h
        src/Symbol.h
        src/SymbolPos.h
        src/Lexer.cpp
        src/ErrorManager.cpp
        src/ErrorManager.h
)

target_link_libraries(PDL PRIVATE ICU::uc)
