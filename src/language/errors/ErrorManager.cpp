#include "ErrorManager.h"

#include "language/process/Lexer.h"
#include "language/process/Parser.h"

#include <iostream>

namespace {
    void PrintHint(std::string_view line, unsigned column, unsigned length) {
        std::string displayHint;
        for (unsigned i = 1; i < column; i++) {
            displayHint += " ";
        }
        for (unsigned i = 0; i < length; i++) {
            displayHint += "^";
        }

        std::cerr << " ->   " << line << std::endl;
        std::cerr << "      " << displayHint << std::endl;
    }
}

void ErrorManager::ProcessLexicalException(Lexer& lexer, const LexicalException& e) {
    std::cerr << std::format(
        "({}:{}) LE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    PrintHint(lexer.GetCurrentLine(), e.GetColumn(), 1);

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
        break;
    }
}

void ErrorManager::ProcessSyntaxException(Lexer& lexer, const SyntaxException& e) {
    std::cerr << std::format(
        "({}:{}) SE-{:04X}: ",
        e.GetLine(), e.GetColumn(),
        static_cast<uint32_t>(e.GetCode())
    );
    std::cerr << e.what() << std::endl;

    lexer.SkipLine();
    PrintHint(lexer.GetCurrentLine(), e.GetColumn(), e.GetLength());

    m_status = 2;
    throw CriticalLanguageException();
}

void ErrorManager::LogSemanticError(
    std::string_view currentLine,
    unsigned line, unsigned column,
    unsigned length,
    SemanticError error,
    std::string_view message
) {
    std::cerr << std::format(
        "({}:{}) UE-{:04X}: ",
        line, column,
        static_cast<uint32_t>(error)
    );
    std::cerr << message << std::endl;

    if (length > 0) {
        PrintHint(currentLine, column, length);
    }

    m_status = 1;
}

void ErrorManager::LogError(std::string_view message) {
    m_status = 1;
    std::cerr << message << std::endl;
}
