#include "Lexer.h"

void Lexer::ReadDelAndComments() {
    while (true) {
        // 0 : del : 0
        if (IsSpaceAscii(m_lastChar)) {
            Read();
        }
        // 0 : / : 15
        else if (m_lastChar == '/') {
            Read();

            // 15 : * : 16
            if (m_lastChar != '*')
                ThrowLexicalError(LexicalError::MISSING_COMMENT_START);

            Read();

            // Procesamos el resto de transiciones como un bucle.
            bool m_loop = true;
            bool m_awaitExit = false;
            while (m_loop) {
                if (m_lastChar == EOF)
                    ThrowLexicalError(LexicalError::MISSING_COMMENT_END);

                if (m_awaitExit) {
                    // 17 : / : 0
                    if (m_lastChar == '/') {
                        Read();
                        m_loop = false;
                    }
                    // 17 : * : 17
                    else if (m_lastChar == '*') {
                        Read();
                    }
                    // 17 : cc : 16
                    else {
                        m_awaitExit = false;
                        Read();
                    }
                } else {
                    if (m_lastChar == '*') {
                        m_awaitExit = true;
                        Read();
                    } else {
                        m_awaitExit = false;
                        Read();
                    }
                }
            }
        }

        // Otra transición. Salimos del bucle.
        else break;
    }
}

Token Lexer::ReadToken(GlobalState& globals) {
    ReadDelAndComments();

    m_tokenLine = m_line;
    m_tokenColumn = m_column;

    // 0 : l : 1
    if (IsAlphaUnicode(m_lastChar)) {
        std::string lex;
        lex += static_cast<char>(m_lastChar);

        Read();

        // 1 : l, d, _ : 1
        while (IsAlnumUnicode(m_lastChar) || m_lastChar == '_') {
            lex += static_cast<char>(m_lastChar);
            Read();
        }

        // 1 : oc : 2
        const TokenType type = KeywordToToken(lex);

        if (type != TokenType::IDENTIFIER)
            return CreateToken(type);

        auto pos = globals.SearchSymbol(lex);

        if (!pos.has_value()) {
            if (globals.implicitDeclaration) {
                const auto newPos = globals.AddGlobalSymbol(lex);
                globals.AddType(newPos, ValueProduct { tInt });
                globals.AddOffset(newPos, globals.globalOffset);
                globals.globalOffset += wInt;
                pos = newPos;
            } else {
                pos = globals.AddSymbol(lex);
            }
        }

        return CreateToken(type, pos.value());
    }

    // 0 : d : 3
    if (IsDigitAscii(m_lastChar)) {
        int32_t num = static_cast<unsigned char>(m_lastChar) - '0';
        bool numberTooBig = false;
        Read();

        // 3 : d : 3
        while (IsDigitAscii(m_lastChar)) {
            if (!numberTooBig) {
                num = num * 10 + (static_cast<unsigned char>(m_lastChar) - '0');

                if (num > 32767) {
                    numberTooBig = true;
                }
            }

            Read();
        }

        // 3 : oc : 4
        if (numberTooBig)
            ThrowLexicalError(LexicalError::INT_TOO_BIG);

        return CreateToken(TokenType::CINT, static_cast<int16_t>(num));
    }

    // 0 : ' : 5
    if (m_lastChar == '\'') {
        std::string str;
        size_t counter = 0;
        Read();

        while (m_lastChar != '\'') {
            if (m_lastChar == static_cast<char32_t>(EOF))
                ThrowLexicalError(LexicalError::MISSING_STRING_END);

            // 5 : \ : 6
            if (m_lastChar == '\\') {
                Read();

                // 6 : cesc : 5
                const int escapedChar = EscapedToAscii(m_lastChar);

                // Carácter ilegal.
                if (escapedChar == -1)
                    ThrowLexicalError(LexicalError::STRING_ESCAPE_SEQUENCE);

                str += CodepointToUtf8(escapedChar);
                counter += 1;
                Read();
            }
            // 5 : oc : 5
            else if (!IsAscii(m_lastChar) || IsPrintAscii(m_lastChar) || m_lastChar == ' ') {
                str += CodepointToUtf8(m_lastChar);
                counter += 1;
                Read();
            }
            // Carácter ilegal.
            else {
                ThrowLexicalError(LexicalError::STRING_FORBIDDEN_CHARACTER);
            }
        }

        // 5 : ' : 7

        // Comprobamos el contador y, si es mayor que 64, lanzamos un error.
        if (counter > 64)
            ThrowLexicalError(LexicalError::STRING_TOO_LONG);

        Read();
        return CreateToken(TokenType::CSTR, str);
    }

    // 0 : + : 8
    if (m_lastChar == '+') {
        Read();

        // 8 : oc : 9
        if (m_lastChar != '=')
            return CreateToken(TokenType::SUM);

        // 8 : = : 9
        Read();
        return CreateToken(TokenType::CUMULATIVE_ASSIGN);
    }

    // 0 : - : 10
    if (m_lastChar == '-') {
        Read();
        return CreateToken(TokenType::SUB);
    }

    // 0 : = : 10
    if (m_lastChar == '=') {
        Read();
        return CreateToken(TokenType::ASSIGN);
    }

    // 0 : < : 10
    if (m_lastChar == '<') {
        Read();
        return CreateToken(TokenType::LESS);
    }

    // 0 : > : 10
    if (m_lastChar == '>') {
        Read();
        return CreateToken(TokenType::GREATER);
    }

    // 0 : & : 11
    if (m_lastChar == '&') {
        Read();

        // 11 : & : 12
        if (m_lastChar != '&')
            ThrowLexicalError(LexicalError::MISSING_OP_AND);

        Read();
        return CreateToken(TokenType::AND);
    }

    // 0 : | : 13
    if (m_lastChar == '|') {
        Read();
        if (m_lastChar != '|')
            ThrowLexicalError(LexicalError::MISSING_OP_OR);

        Read();
        return CreateToken(TokenType::OR);
    }

    // 0 : , : 18
    if (m_lastChar == ',') {
        Read();
        return CreateToken(TokenType::COMMA);
    }

    // 0 : ; : 18
    if (m_lastChar == ';') {
        Read();
        return CreateToken(TokenType::SEMICOLON);
    }

    // 0 : ( : 18
    if (m_lastChar == '(') {
        Read();
        return CreateToken(TokenType::PARENTHESIS_OPEN);
    }

    // 0 : ) : 18
    if (m_lastChar == ')') {
        Read();
        return CreateToken(TokenType::PARENTHESIS_CLOSE);
    }

    // 0 : { : 18
    if (m_lastChar == '{') {
        Read();
        return CreateToken(TokenType::CURLY_BRACKET_OPEN);
    }

    // 0 : } : 18
    if (m_lastChar == '}') {
        Read();
        return CreateToken(TokenType::CURLY_BRACKET_CLOSE);
    }

    // 0 : eof : 19
    if (m_lastChar == EOF)
        return CreateToken(TokenType::END);

    // Carácter desconocido.
    ThrowLexicalError(LexicalError::UNEXPECTED_START_CHARACTER);
}
