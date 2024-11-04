#pragma once

#include "Lexer.h"

class Parser
{
    Lexer m_lexer;

    static constexpr bool Compare(const TokenType a, const TokenType b)
    {
        return a == b;
    }

    void Axiom()
    {

    }

public:
    explicit Parser(std::istream& input) : m_lexer(input)
    {
    }
};
