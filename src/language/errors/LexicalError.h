#pragma once

#include "language/core/Characters.h"

#include <stdexcept>

enum class LexicalError : uint32_t {
    MISSING_COMMENT_START = 0x0000,
    MISSING_COMMENT_END = 0x0001,
    MISSING_STRING_END = 0x0010,
    MISSING_OP_AND = 0x0020,
    MISSING_OP_OR = 0x0021,

    STRING_FORBIDDEN_CHARACTER = 0x0100,
    STRING_ESCAPE_SEQUENCE = 0x0101,
    STRING_TOO_LONG = 0x0102,

    INT_TOO_BIG = 0x0200,

    UNEXPECTED_START_CHARACTER = 0xFF00
};

struct LexicalErrorData {
    char32_t invalidCharacter;
};

constexpr std::string GetErrorMessage(LexicalError error, char32_t invalidCharacter) {
    using enum LexicalError;

    switch (error) {
    case MISSING_COMMENT_START:
        return "Carácter inesperado tras «/». Se esperaba «*» para iniciar el comentario de bloque.";
    case MISSING_COMMENT_END:
        return "Fin de fichero inesperado. Se esperaba «*/» para cerrar el comentario de bloque.";
    case MISSING_STRING_END:
        return "Fin de fichero inesperado. Se esperaba «'» para cerrar la cadena.";
    case MISSING_OP_AND:
        return "Se esperaba «&» después de «&» para formar un operador.";
    case MISSING_OP_OR:
        return "Se esperaba «|» después «|» para formar un operador.";

    case STRING_ESCAPE_SEQUENCE:
        return std::format(
            "Error en la cadena. La secuencia de escape '\\{}' (U+{:04X}) no es válida.",
            IsPrintUnicode(invalidCharacter) ? CodepointToUtf8(invalidCharacter) : "\uFFFD",
            static_cast<int32_t>(invalidCharacter)
        );
    case STRING_FORBIDDEN_CHARACTER:
        return std::format(
            "Error en la cadena. Carácter no permitido («{}», U+{:04X}).",
            IsPrintUnicode(invalidCharacter) ? CodepointToUtf8(invalidCharacter) : "\uFFFD",
            static_cast<int32_t>(invalidCharacter)
        );
    case STRING_TOO_LONG:
        return "La longitud de cadena excede el límite de 64 caracteres";

    case INT_TOO_BIG:
        return "El valor del entero es demasiado grande (máximo 32767).";

    case UNEXPECTED_START_CHARACTER:
        return std::format(
            "Carácter inesperado al buscar el siguiente símbolo («{}», U+{:04X}).",
            IsPrintUnicode(invalidCharacter) ? CodepointToUtf8(invalidCharacter) : "\uFFFD",
            static_cast<int32_t>(invalidCharacter)
        );

    default:
        return "";
    }
}

class LexicalException final : public std::runtime_error {
    LexicalError m_error;
    unsigned m_lineNumber;
    unsigned m_columnNumber;

public:
    constexpr LexicalException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        LexicalError error,
        char32_t invalidCharacter
    ) noexcept
        : runtime_error(GetErrorMessage(error, invalidCharacter)),
          m_error(error),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    [[nodiscard]] LexicalError GetCode() const { return m_error; }
    [[nodiscard]] unsigned GetLine() const { return m_lineNumber; }
    [[nodiscard]] unsigned GetColumn() const { return m_columnNumber; }
};
