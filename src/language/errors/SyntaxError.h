#pragma once

#include "language/core/Token.h"

#include <stdexcept>
#include <format>
#include <utility>

enum class SyntaxError : uint32_t {
    TOP_LEVEL_INVALID = 0x0100,

    FUNCTION_INVALID = 0x200,
    FUNCTION_MISSING_IDENTIFIER = 0x0210,
    FUNCTION_MISSING_PAREN_OPEN = 0x0211,
    FUNCTION_MISSING_PAREN_CLOSE = 0x0212,
    FUNCTION_MISSING_BRACK_OPEN = 0x0213,
    FUNCTION_MISSING_BRACK_CLOSE = 0x0214,

    FUNTYPE_INVALID = 0x0300,

    VARTYPE_INVALID = 0x0400,
    VARTYPE_VOID = 0x0410,

    FUNATTRIBUTES_INVALID = 0x0500,
    FUNATTRIBUTES_EMPTY = 0x0510,
    FUNATTRIBUTES_MISSING_IDENTIFIER = 0x0520,

    STATEMENT_INVALID = 0x0600,
    STATEMENT_MISSING_END_SEMICOLON = 0x0610,
    STATEMENT_IF_MISSING_PAREN_OPEN = 0x0620,
    STATEMENT_IF_MISSING_PAREN_CLOSE = 0x0621,
    STATEMENT_FOR_MISSING_PAREN_OPEN = 0x0630,
    STATEMENT_FOR_MISSING_SEMICOLON = 0x0631,
    STATEMENT_FOR_MISSING_PAREN_CLOSE = 0x0632,
    STATEMENT_FOR_MISSING_BRACK_OPEN = 0x0633,
    STATEMENT_FOR_MISSING_BRACK_CLOSE = 0x0634,
    STATEMENT_VAR_MISSING_IDENTIFIER = 0x0640,
    STATEMENT_INPUT_MISSING_IDENTIFIER = 0x0650,

    IDACT_INVALID = 0x0700,
    IDACT_CALL_MISSING_PAREN_CLOSE = 0x0710,

    ASS_INVALID = 0x0800,

    EXP_INVALID = 0x0900,
    EXP_MISSING_PAREN_CLOSE = 0x0901,

    COMP_INVALID = 0x0A00,

    ARITH_INVALID = 0x0B00
};

constexpr std::string GetErrorMessage(SyntaxError error, const Token& token) {
    using enum SyntaxError;

    const auto& type = token.type;

    switch (error) {
    case TOP_LEVEL_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado en el nivel superior. Se esperaba una "
            "declaración de función o una sentencia de nivel superior.",
            GetDisplayString(type)
        );

    case FUNCTION_INVALID:
        std::unreachable();
    case FUNCTION_MISSING_IDENTIFIER:
        return std::format(
            "Elemento de tipo «{}» inesperado como nombre de función. Se esperaba un "
            "identificador válido.",
            GetDisplayString(type)
        );
    case FUNCTION_MISSING_PAREN_OPEN:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «(» tras el nombre para "
            "definir los atributos de la función.",
            GetDisplayString(type)
        );
    case FUNCTION_MISSING_PAREN_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «)» para terminar de definir "
            "los atributos.",
            GetDisplayString(type)
        );
    case FUNCTION_MISSING_BRACK_OPEN:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «{{» tras los atributos para "
            "definir el cuerpo de la función.",
            GetDisplayString(type)
        );
    case FUNCTION_MISSING_BRACK_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «}}» para terminar de definir "
            "el cuerpo de la función.",
            GetDisplayString(type)
        );
    case FUNTYPE_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba el tipo de retorno de "
            "la función («int», «boolean», «string» o «void»).",
            GetDisplayString(type)
        );

    case VARTYPE_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba un tipo de variable válido "
            "(«int», «boolean» o «string»).",
            GetDisplayString(type)
        );
    case VARTYPE_VOID:
        return
            "Una variable no puede ser de tipo «void». Se esperaba un tipo válido "
            "(«int», «boolean» o «string»)";

    case FUNATTRIBUTES_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba un tipo para comenzar a "
            "definir la lista de atributos.",
            GetDisplayString(type)
        );
    case FUNATTRIBUTES_EMPTY:
        return
            "Es necesario definir algún atributo para la función, o «void» si no "
            "toma argumentos.";
    case FUNATTRIBUTES_MISSING_IDENTIFIER:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se espera el nombre del atributo tras "
            "declarar su tipo.",
            GetDisplayString(type)
        );

    case STATEMENT_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se espera una sentencia válida (p. ej.,"
            "«if», «for», «input» o una asignación).",
            GetDisplayString(type)
            );
    case STATEMENT_MISSING_END_SEMICOLON:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «;» para terminar la sentencia.",
            GetDisplayString(type)
        );
    case STATEMENT_IF_MISSING_PAREN_OPEN:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «(» tras «if» para definir "
            "la condición de «if».",
            GetDisplayString(type)
        );
    case STATEMENT_IF_MISSING_PAREN_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «)» tras terminar de definir "
            "la condición de «if».",
            GetDisplayString(type)
        );
    case STATEMENT_FOR_MISSING_PAREN_OPEN:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «(» tras «for» para definir "
            "los argumentos de «for».",
            GetDisplayString(type)
        );
    case STATEMENT_FOR_MISSING_SEMICOLON:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «;» para separar entre sí los"
            "argumentos de «for».",
            GetDisplayString(type)
        );
    case STATEMENT_FOR_MISSING_PAREN_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «)» tras terminar de definir "
            "los argumentos de «for».",
            GetDisplayString(type)
        );
    case STATEMENT_FOR_MISSING_BRACK_OPEN:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «{{» tras los argumentos para "
            "definir el cuerpo de «for».",
            GetDisplayString(type)
        );
    case STATEMENT_FOR_MISSING_BRACK_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «}}» tras terminar de definir "
            "el cuerpo de «for».",
            GetDisplayString(type)
            );
    case STATEMENT_VAR_MISSING_IDENTIFIER:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba un identificador tras el tipo "
            "de la variable.",
            GetDisplayString(type)
        );
    case STATEMENT_INPUT_MISSING_IDENTIFIER:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba un identificador para recibir "
            "la entrada.",
            GetDisplayString(type)
            );

    case IDACT_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se debe realizar una asignación o llamada "
            "sobre el identificador.",
            GetDisplayString(type)
        );
    case IDACT_CALL_MISSING_PAREN_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Falta «)» para finalizar la llamada a la "
            "función.",
            GetDisplayString(type)
        );

    case ASS_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «=» o «+=» para la asignación.",
            GetDisplayString(type)
        );

    case EXP_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Una expresión formada así no es correcta.",
            GetDisplayString(type)
        );
    case EXP_MISSING_PAREN_CLOSE:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «)» para terminar la expresión.",
            GetDisplayString(type)
        );

    case COMP_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «<» o «>» para la comparación.",
            GetDisplayString(type)
        );

    case ARITH_INVALID:
        return std::format(
            "Elemento de tipo «{}» inesperado. Se esperaba «+» o «-» para la operación"
            "aritmética.",
            GetDisplayString(type)
        );
    //
    //
    }

    std::unreachable();
}

class SyntaxException final : public std::runtime_error {
    SyntaxError m_error;
    Token m_token;

public:
    [[deprecated]]
    SyntaxException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const std::string& message
    ) noexcept
        : runtime_error(message),
          m_error(),
          m_token(TokenType::END, lineNumber, columnNumber) {}

    SyntaxException(
        const SyntaxError error,
        const Token& token
    ) noexcept
        : runtime_error(GetErrorMessage(error, token)),
          m_error(error),
          m_token(token) {}

    [[nodiscard]] SyntaxError GetCode() const { return m_error; }
    [[nodiscard]] unsigned GetLine() const { return m_token.line; }
    [[nodiscard]] unsigned GetColumn() const { return m_token.column; }
    [[nodiscard]] unsigned GetLength() const { return m_token.length; }
};
