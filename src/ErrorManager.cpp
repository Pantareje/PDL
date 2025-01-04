#include "ErrorManager.h"

#include "Lexer.h"

#include <iostream>

void ErrorManager::ProcessLexicalException(Lexer& lexer, const LexicalException& e) {
    m_hasError = true;
    std::cerr << std::format(
        "({}:{}) LE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    switch (m_lexicalMode) {
    case LexicalRecoveryMode::SkipLine:
        lexer.SkipLine();
        break;
    case LexicalRecoveryMode::SkipChar:
        lexer.SkipChar();
        break;
    }
}

void ErrorManager::ProcessSyntaxException([[maybe_unused]] Parser& parser, const SyntaxException& e) {
    m_hasError = true;
    std::cerr << std::format(
        "({}:{}) SE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    // Recuperar de un error sintáctico es mucho más complicado que uno léxico o semántico.
}

void ErrorManager::LogSemanticError(unsigned line, unsigned column, std::string_view message) {
    m_hasError = true;
    std::cerr << std::format(
        "({}:{}) UE-{:04X}: ",
        line, column,
        0xFFFF
    );
    std::cerr << message << std::endl;
}
