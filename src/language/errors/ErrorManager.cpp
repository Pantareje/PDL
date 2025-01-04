#include "ErrorManager.h"

#include "language/process/Lexer.h"
#include "language/process/Parser.h"

#include <iostream>

void ErrorManager::ProcessLexicalException(Lexer& lexer, const LexicalException& e) {
    std::cerr << std::format(
        "({}:{}) LE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    switch (m_lexicalMode) {
    case LexicalRecoveryMode::Critical:
        m_status = 2;
        throw CriticalLanguageException();
    case LexicalRecoveryMode::SkipLine:
        m_status = 1;
        lexer.SkipLine();
        break;
    case LexicalRecoveryMode::SkipChar:
        m_status = 1;
        lexer.SkipChar();
        break;
    }
}

void ErrorManager::ProcessSyntaxException([[maybe_unused]] Parser& parser, const SyntaxException& e) {
    std::cerr << std::format(
        "({}:{}) SE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    m_status = 2;
    throw CriticalLanguageException();
}

void ErrorManager::LogSemanticError(unsigned line, unsigned column, std::string_view message) {
    std::cerr << std::format(
        "({}:{}) UE-{:04X}: ",
        line, column,
        0xFFFF
    );

    m_status = 1;
    std::cerr << message << std::endl;
}
