#pragma once

#include "language/errors/LexicalError.h"
#include "language/errors/SyntaxError.h"
#include "language/errors/SemanticError.h"

class Lexer;
class Parser;

enum class LexicalRecoveryMode {
    Critical, SkipChar, SkipLine
};

class CriticalLanguageException final : public std::runtime_error {
public:
    CriticalLanguageException() : std::runtime_error("Error irrecuperable al procesar la entrada.") {}
};

class ErrorManager {
    LexicalRecoveryMode m_lexicalMode = LexicalRecoveryMode::SkipChar;
    int m_status = 0;

public:
    ErrorManager() = default;

    void SetLexicalRecoveryMode(LexicalRecoveryMode mode) { m_lexicalMode = mode; }

    void ProcessLexicalException(Lexer& lexer, const LexicalException& e);
    void ProcessSyntaxException(Lexer& lexer, const SyntaxException& e);

    void LogSemanticError(
        std::string_view currentLine,
        unsigned line, unsigned column,
        unsigned length,
        SemanticError error,
        std::string_view message
    );

    [[nodiscard]] bool GetStatus() const { return m_status; }
};
