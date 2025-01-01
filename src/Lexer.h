#pragma once

#include "Token.h"
#include "LexicalError.h"
#include "GlobalState.h"

class Lexer {
    std::istream& m_input;
    char32_t m_lastChar = U' ';

    unsigned m_line = 1;
    unsigned m_column = 0;

    unsigned m_tokenLine = m_line;
    unsigned m_tokenColumn = m_column;

    [[noreturn]]
    void ThrowError(LexicalError error) const {
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

    Token GetToken(GlobalState& semanticState) {
        return ReadToken(semanticState);
    }

    void SkipLine() {
        while (m_lastChar != U'\n' && m_lastChar != EOF) {
            Read();
        }
    }
};
