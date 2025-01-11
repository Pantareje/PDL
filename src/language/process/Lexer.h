#pragma once

#include "language/core/Token.h"
#include "language/errors/LexicalError.h"
#include "language/core/GlobalState.h"

class Lexer {
    std::istream& m_input;
    char32_t m_lastChar = U' ';

    std::basic_string<char32_t> m_currentLine;

    bool m_newLine = true;

    unsigned m_line = 0;
    unsigned m_column = 0;

    unsigned m_tokenColumn = 0;

    [[noreturn]]
    void ThrowLexicalError(LexicalError error) const {
        throw LexicalException(m_line, m_column, error, m_lastChar);
    }

    template<typename T>
    constexpr Token CreateToken(TokenType tokenType, T&& attribute) {
        assert(m_column > m_tokenColumn);
        return Token {
            .type = tokenType,
            .line = m_line,
            .column = m_tokenColumn,
            .length = m_column - m_tokenColumn,
            .attribute = attribute
        };
    }

    constexpr Token CreateToken(TokenType tokenType) {
        return CreateToken(tokenType, std::monostate());
    }

    void Read() {
        if (m_newLine) {
            m_column = 0;
            m_line += 1;
            m_currentLine.clear();
            m_newLine = false;
        }

        m_lastChar = ReadUtf8Codepoint(m_input);
        m_column += 1;

        if (m_lastChar == '\n') {
            m_newLine = true;
        } else {
            m_currentLine += m_lastChar;
        }

#ifndef NDEBUG
        if (m_lastChar == U'€') {
            m_lastChar = static_cast<char32_t>(EOF);
        }
#endif
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

    std::string GetCurrentLine() const {
        std::string result;

        for (const char32_t& c : m_currentLine) {
            result += CodepointToUtf8(c);
        }

        return result;
    }
};
