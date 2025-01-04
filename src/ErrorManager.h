#pragma once

#include "LexicalError.h"
#include "SyntaxException.h"

class Lexer;
class Parser;

enum class LexicalRecoveryMode {
    SkipChar, SkipLine
};

class ErrorManager {
    LexicalRecoveryMode m_lexicalMode = LexicalRecoveryMode::SkipLine;
    bool m_hasError = false;

public:
    ErrorManager() = default;

    void SetLexicalRecoveryMode(LexicalRecoveryMode mode) { m_lexicalMode = mode; }

    void ProcessLexicalException(Lexer& lexer, const LexicalException& e);
    void ProcessSyntaxException(Parser& parser, const SyntaxException& e);

    void LogSemanticError(unsigned line, unsigned column, std::string_view message);

    [[nodiscard]] bool HasError() const { return m_hasError; }
};
