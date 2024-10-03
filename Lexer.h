#pragma once

#include "Token.h"

#include <istream>

class Lexer {
    std::istream& m_input;
    int m_lastChar = ' ';

    void Read() { m_lastChar = m_input.get(); }

    static bool IsCommentChar(const int c) { return std::isgraph(c) || std::isspace(c); }

    static int EscapeToAscii(const int c) {
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
            if (std::isspace(m_lastChar))
                Read();

                // 0 : / : 15
            else if (m_lastChar == '/') {
                Read();

                // 15 : * : 16
                if (m_lastChar != '*') throw std::exception();
                Read();

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
                        else if (IsCommentChar(m_lastChar)) {
                            m_awaitExit = false;
                            Read();
                        } else {
                            throw std::exception();
                        }
                    } else {
                        if (m_lastChar == '*') {
                            m_awaitExit = true;
                            Read();
                        } else if (IsCommentChar(m_lastChar)) {
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
        if (std::isalpha(m_lastChar)) {
            std::string lex;
            lex += static_cast<char>(m_lastChar);

            Read();

            // 1 : l, d, _ : 1
            while (std::isalnum(m_lastChar) || m_lastChar == '_') {
                lex += static_cast<char>(m_lastChar);
                Read();
            }

            // 1 : oc : 2
            // TODO: Tabla de símbolos y mirar si es palabra reservada
            return { TokenType::IDENTIFIER, lex };
        }

        // 0 : d : 3
        if (std::isdigit(m_lastChar)) {
            int32_t num = m_lastChar - '0';
            Read();

            // 3 : d : 3
            while (std::isdigit(m_lastChar)) {
                num = num * 10 + (m_lastChar - '0');
                Read();
            }

            // 3 : oc : 4
            return { TokenType::CINT, num };
        }

        // 0 : ' : 5
        if (m_lastChar == '\'') {
            // std::string tiene un contador interno, que se incrementa
            // en uno cada vez que añadimos un carácter.
            std::string lex;
            Read();

            while (m_lastChar != '\'') {
                if (!std::isprint(m_lastChar)) throw std::exception();

                // 5 : \ : 6
                if (m_lastChar == '\\') {
                    Read();

                    // 6 : cesc : 5
                    const int escapedChar = EscapeToAscii(m_lastChar);
                    if (escapedChar == -1) throw std::exception();
                    lex += static_cast<char>(escapedChar);
                    Read();
                }
                // 5 : oc : 5
                else {
                    lex += static_cast<char>(m_lastChar);
                    Read();
                }
            }

            // Comprobamos el contador y, si es mayor que 64, lanzamos un error.
            if (lex.size() > 64) throw std::exception();

            // 5 : ' : 7
            Read();
            return { TokenType::CSTR, lex };
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
        throw std::exception();
    }
};
