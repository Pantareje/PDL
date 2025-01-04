#pragma once

#include "language/core/Token.h"
#include "language/errors/LexicalError.h"
#include "language/core/GlobalState.h"

class Lexer {
    std::istream& m_input;
    char32_t m_lastChar = U' ';

    unsigned m_line = 1;
    unsigned m_column = 0;

    unsigned m_tokenLine = m_line;
    unsigned m_tokenColumn = m_column;

    [[noreturn]]
    void ThrowLexicalError(LexicalError error) const {
        throw LexicalException(m_line, m_column, error, m_lastChar);
    }

    template<typename T>
    constexpr Token CreateToken(TokenType tokenType, T&& args) {
        return { tokenType, m_tokenLine, m_tokenColumn, args };
    }

    constexpr Token CreateToken(TokenType tokenType) {
        return CreateToken(tokenType, std::monostate());
    }

    void Read() {
        m_lastChar = ReadUtf8Codepoint(m_input);

        if (m_lastChar == '\n') {
            m_column = 0;
            m_line += 1;
        } else {
            m_column += 1;
        }

        if (m_lastChar == U'€') {
            m_lastChar = static_cast<char32_t>(EOF);
        }
    }

    void ReadDelAndComments();

    Token ReadToken(GlobalState& globals);

public:
    explicit Lexer(std::istream& input) : m_input(input) {}

    Token GetToken(GlobalState& globals) {
        for (size_t i = 0; i < 5000; i++) {
            try {
                return ReadToken(globals);
            } catch (const LexicalException& e) {
                globals.errorManager.ProcessLexicalException(*this, e);
            }
        }

        throw CriticalLanguageException();
    }

    void SkipLine() {
        while (m_lastChar != U'\n' && m_lastChar != EOF) {
            Read();
        }
    }

    void SkipChar() {
        Read();
    }
};
