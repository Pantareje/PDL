cmake_minimum_required(VERSION 3.28)
project(PDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ICU REQUIRED COMPONENTS uc)

add_executable(PDL
        src/Main.cpp
        src/application/ApplicationAttributes.h
        src/application/Application.h
        src/application/Application.cpp
        src/language/core/Characters.h
        src/language/core/Characters.cpp
        src/language/core/Token.h
        src/language/core/ValueProduct.h
        src/language/core/Symbol.h
        src/language/core/SymbolPos.h
        src/language/core/SymbolTable.h
        src/language/core/GlobalState.h
        src/language/errors/LexicalError.h
        src/language/errors/SyntaxError.h
        src/language/errors/ErrorManager.h
        src/language/errors/ErrorManager.cpp
        src/language/process/Lexer.h
        src/language/process/Lexer.cpp
        src/language/process/Parser.h
        src/language/process/Parser.cpp
)

target_include_directories(PDL PRIVATE src)

target_link_libraries(PDL PRIVATE ICU::uc)
