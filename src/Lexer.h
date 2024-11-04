#pragma once

#include "Characters.h"
#include "Token.h"
#include "SymbolTable.h"
#include "LexicalException.h"

#include <format>


class Lexer {
    std::istream& m_input;
    char32_t m_lastChar = U' ';

    unsigned m_line = 1;
    unsigned m_column = 0;

    void Read() {
        m_lastChar = ReadUtf8Codepoint(m_input);

        if (m_lastChar == '\n') {
            m_column = 0;
            m_line += 1;
        } else {
            m_column += 1;
        }
    }

    void ReadDelAndComments() {
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
                    throw LexicalException(
                        m_line, m_column,
                        "Carácter inesperado tras «/». Se esperaba «*» para iniciar un comentario de bloque."
                    );

                Read();

                // Procesamos el resto de transiciones como un bucle.
                bool m_loop = true;
                bool m_awaitExit = false;
                while (m_loop) {
                    if (m_lastChar == EOF)
                        throw LexicalException(
                            m_line, m_column,
                            "Fin de fichero inesperado. Se esperaba «*/» para cerrar el comentario de bloque.");

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

public:
    explicit Lexer(std::istream& input) : m_input(input) {}

    Token GetToken(SymbolTable& symbolTable) {
        ReadDelAndComments();

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
                return { type };


            auto pos = symbolTable.SearchSymbol(lex);

            if (!pos.has_value())
                pos = { symbolTable.AddSymbol(lex) };

            return { type, pos.value() };
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
                throw LexicalException(
                    m_line, m_column,
                    "El valor del entero es demasiado grande (máximo 32767)."
                );

            return { TokenType::CINT, static_cast<int16_t>(num) };
        }

        // 0 : ' : 5
        if (m_lastChar == '\'') {
            std::string str;
            size_t counter = 0;
            Read();

            while (m_lastChar != '\'') {
                if (m_lastChar == static_cast<char32_t>(EOF))
                    throw LexicalException(
                        m_line, m_column,
                        "Fin de fichero inesperado. Se esperaba «'» para cerrar la cadena."
                    );

                // 5 : \ : 6
                if (m_lastChar == '\\') {
                    Read();

                    // 6 : cesc : 5
                    const int escapedChar = EscapedToAscii(m_lastChar);

                    // Carácter ilegal.
                    if (escapedChar == -1)
                        throw LexicalException(
                            m_line, m_column,
                            m_lastChar == ' ' || IsPrintUnicode(m_lastChar)
                                ? std::format(
                                    "Error en la cadena. La secuencia de escape '\\{}' (U+{:04X}) no es válida.",
                                    CodepointToUtf8(m_lastChar),
                                    static_cast<int32_t>(m_lastChar)
                                )
                                : std::format(
                                    "Error en la cadena. Carácter ilegal en la secuencia de escape (U+{:04X}).",
                                    static_cast<int32_t>(m_lastChar)
                                )

                        );

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
                    throw LexicalException(
                        m_line, m_column,
                        IsPrintUnicode(m_lastChar)
                            ? std::format(
                                "Error en la cadena. Carácter no permitido («{}», U+{:04X}).",
                                CodepointToUtf8(m_lastChar),
                                static_cast<int32_t>(m_lastChar)
                            )
                            : std::format(
                                "Error en la cadena. Carácter no permitido (U+{:04X}).",
                                static_cast<int32_t>(m_lastChar)
                            )
                    );
                }
            }

            // 5 : ' : 7

            // Comprobamos el contador y, si es mayor que 64, lanzamos un error.
            if (counter > 64)
                throw LexicalException(
                    m_line, m_column,
                    std::format(
                        "La longitud de cadena excede el límite de 64 caracteres ({} caracteres).",
                        counter
                    )
                );

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
            if (m_lastChar != '&')
                throw LexicalException(
                    m_line, m_column,
                    "Se esperaba «&» después de «&» para formar un operador."
                );

            Read();
            return { TokenType::AND };
        }

        // 0 : | : 13
        if (m_lastChar == '|') {
            Read();
            if (m_lastChar != '|')
                throw LexicalException(
                    m_line, m_column,
                    "Se esperaba «|» después «|» para formar un operador."
                );

            Read();
            return { TokenType::OR };
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
        if (m_lastChar == EOF || m_lastChar == U'€')
            return { TokenType::END };

        // Carácter desconocido.
        throw LexicalException(
            m_line, m_column,
            IsPrintUnicode(m_lastChar)
                ? std::format(
                    "Carácter inesperado al buscar el siguiente símbolo («{}», U+{:04X}).",
                    CodepointToUtf8(m_lastChar),
                    static_cast<int32_t>(m_lastChar)
                )
                : std::format(
                    "Carácter inesperado al buscar el siguiente símbolo (U+{:04X}).",
                    static_cast<int32_t>(m_lastChar)
                )
        );
    }

    void SkipChar() { Read(); }
};
