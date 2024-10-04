#pragma once

#include "Characters.h"
#include "Token.h"
#include "LexicalException.h"

#include <format>

class Lexer {
    std::istream& m_input;
    char32_t m_lastChar = U' ';

    unsigned m_line = 0;
    unsigned m_column = 0;

    void Read() {
        m_lastChar = ReadUtf8(m_input);

        if (m_lastChar == '\n') {
            m_column = 0;
            m_line += 1;
        } else {
            m_column += 1;
        }
    }

    static int EscapeToAscii(char32_t c) {
        switch (c) {
        case '\\': return '\\';
        case '\'': return '\'';

        case '0': return '\0';

        case 'n': return '\n';
        case 'a': return '\a';
        case 't': return '\t';
        case 'v': return '\v';
        case 'f': return '\f';
        case 'r': return '\r';

        default: return -1;
        }
    }

    void ReadDelAndComments() {
        while (true) {
            // 0 : del : 0
            if (IsSpace(m_lastChar))
                Read();

            // 0 : / : 15
            else if (m_lastChar == '/') {
                Read();

                // 15 : * : 16
                if (m_lastChar != '*')
                    throw LexicalException(
                        m_line,
                        m_column,
                        "Carácter inesperado tras «/». Se esperaba «*» para hacer un comentario."
                    );

                Read();

                // Procesamos el resto de transiciones como un bucle.
                bool m_loop = true;
                bool m_awaitExit = false;
                while (m_loop) {
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
                        else if (IsSource(m_lastChar)) {
                            m_awaitExit = false;
                            Read();
                        } else {
                            throw std::exception();
                        }
                    } else {
                        if (m_lastChar == '*') {
                            m_awaitExit = true;
                            Read();
                        } else if (IsSource(m_lastChar)) {
                            m_awaitExit = false;
                            Read();
                        } else {
                            throw std::exception();
                        }
                    }
                }
            }

            // Otra transición. Salimos del bucle.
            else break;
        }
    }

public:
    explicit Lexer(std::istream& input) : m_input(input) {}

    Token GetToken() {
        ReadDelAndComments();

        // 0 : l : 1
        if (IsAlpha(m_lastChar)) {
            std::string identifier;
            identifier += static_cast<char>(m_lastChar);

            Read();

            // 1 : l, d, _ : 1
            while (IsAlnum(m_lastChar) || m_lastChar == '_') {
                identifier += static_cast<char>(m_lastChar);
                Read();
            }

            // 1 : oc : 2
            const TokenType type = KeywordToToken(identifier);

            if (type != TokenType::IDENTIFIER)
                return { type };

            // TODO: Tabla de símbolos y mirar si es palabra reservada
            return { type, identifier };
        }

        // 0 : d : 3
        if (IsDigit(m_lastChar)) {
            int32_t num = static_cast<unsigned char>(m_lastChar) - '0';
            Read();

            // 3 : d : 3
            while (IsDigit(m_lastChar)) {
                num = num * 10 + (static_cast<unsigned char>(m_lastChar) - '0');
                Read();
            }

            // 3 : oc : 4
            return { TokenType::CINT, num };
        }

        // 0 : ' : 5
        if (m_lastChar == '\'') {
            std::string str;
            size_t counter = 0;
            Read();

            while (m_lastChar != '\'') {
                if (IsAscii(m_lastChar) && !IsPrint(m_lastChar))
                    throw std::exception();

                // 5 : \ : 6
                if (m_lastChar == '\\') {
                    Read();

                    // 6 : cesc : 5
                    const int escapedChar = EscapeToAscii(m_lastChar);
                    if (escapedChar == -1) throw std::exception();
                    str += ToUtf8(escapedChar);
                    counter += 1;
                    Read();
                }
                // 5 : oc : 5
                else {
                    str += ToUtf8(m_lastChar);
                    counter += 1;
                    Read();
                }
            }

            // Comprobamos el contador y, si es mayor que 64, lanzamos un error.
            if (counter > 64) throw std::exception();

            // 5 : ' : 7
            Read();
            return { TokenType::CSTR, str };
        }

        // 0 : + : 8
        if (m_lastChar == '+') {
            Read();

            // 8 : oc : 9
            if (m_lastChar != '=')
                return { TokenType::SUM };

            // 8 : = : 9
            Read();
            return { TokenType::CUMULATIVE_ASSIGN };
        }

        // 0 : - : 10
        if (m_lastChar == '-') {
            Read();
            return { TokenType::SUB };
        }

        // 0 : = : 10
        if (m_lastChar == '=') {
            Read();
            return { TokenType::ASSIGN };
        }

        // 0 : < : 10
        if (m_lastChar == '<') {
            Read();
            return { TokenType::LESS };
        }

        // 0 : > : 10
        if (m_lastChar == '>') {
            Read();
            return { TokenType::GREATER };
        }

        // 0 : & : 11
        if (m_lastChar == '&') {
            Read();

            // 11 : & : 12
            if (m_lastChar == '&')
                return { TokenType::AND };

            throw std::exception();
        }

        // 0 : | : 13
        if (m_lastChar == '|') {
            Read();
            if (m_lastChar == '|')
                return { TokenType::OR };
            throw std::exception();
        }

        // 0 : , : 18
        if (m_lastChar == ',') {
            Read();
            return { TokenType::COMMA };
        }

        // 0 : ; : 18
        if (m_lastChar == ';') {
            Read();
            return { TokenType::SEMICOLON };
        }

        // 0 : ( : 18
        if (m_lastChar == '(') {
            Read();
            return { TokenType::PARENTHESIS_OPEN };
        }

        // 0 : ) : 18
        if (m_lastChar == ')') {
            Read();
            return { TokenType::PARENTHESIS_CLOSE };
        }

        // 0 : { : 18
        if (m_lastChar == '{') {
            Read();
            return { TokenType::CURLY_BRACKET_OPEN };
        }

        // 0 : } : 18
        if (m_lastChar == '}') {
            Read();
            return { TokenType::CURLY_BRACKET_CLOSE };
        }

        // 0 : eof : 19
        if (m_lastChar == EOF)
            return { TokenType::END };

        // Carácter desconocido.
        throw LexicalException(
            m_line,
            m_column,
            std::format(
                "Carácter inesperado («{}», 0x{:x}).",
                ToUtf8(m_lastChar),
                static_cast<uint64_t>(m_lastChar)
            )
        );
    }

    void SkipChar() { Read(); }
};
