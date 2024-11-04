#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <variant>
#include <format>

/** El tipo del token del analizador léxico. **/
enum class TokenType : unsigned char {
    END,
    BOOL,
    FOR,
    FUNCTION,
    IF,
    INPUT,
    OUTPUT,
    RETURN,
    INT,
    STRING,
    VAR,
    VOID,
    IDENTIFIER,
    ASSIGN,
    CUMULATIVE_ASSIGN,
    SUM,
    SUB,
    AND,
    OR,
    LESS,
    GREATER,
    TRUE,
    FALSE,
    COMMA,
    SEMICOLON,
    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,
    CURLY_BRACKET_OPEN,
    CURLY_BRACKET_CLOSE,
    CINT,
    CSTR
};


/**
 * La unidad básica del analizador léxico. Definido
 * mediante el tipo de token y su contenido.
 */
struct Token {
    /** El identificador del token. **/
    TokenType type {};

    unsigned line;
    unsigned column;

    /**
     * El contenido del token. Dependiendo del tipo puede estar vacío
     * o ser un identificador, una cadena, o un entero de 16 bits con signo.
     */
    std::variant<std::monostate, size_t, std::string, int16_t> attribute {};
};

/**
 * Convierte un token identificador a uno correspondiente
 * a una palabra reservada si es necesaria.
 */
constexpr TokenType KeywordToToken(const std::string_view str) {
    if (str == "boolean") return TokenType::BOOL;
    if (str == "for") return TokenType::FOR;
    if (str == "function") return TokenType::FUNCTION;
    if (str == "if") return TokenType::IF;
    if (str == "input") return TokenType::INPUT;
    if (str == "output") return TokenType::OUTPUT;
    if (str == "int") return TokenType::INT;
    if (str == "return") return TokenType::RETURN;
    if (str == "string") return TokenType::STRING;
    if (str == "var") return TokenType::VAR;
    if (str == "void") return TokenType::VOID;

    if (str == "false") return TokenType::FALSE;
    if (str == "true") return TokenType::TRUE;

    return TokenType::IDENTIFIER;
}

/**
 * Devuelve una cadena con el nombre del token.
 * @param token El tipo del token.
 * @return El identificador del token.
 */
constexpr const char* ToString(const TokenType token) noexcept {
    using enum TokenType;

    switch (token) {
    case END: return "eof";
    case BOOL: return "bool";
    case FOR: return "for";
    case FUNCTION: return "fn";
    case IF: return "if";
    case INPUT: return "in";
    case OUTPUT: return "out";
    case RETURN: return "ret";
    case INT: return "int";
    case STRING: return "str";
    case VAR: return "var";
    case VOID: return "void";
    case IDENTIFIER: return "id";
    case ASSIGN: return "ass";
    case CUMULATIVE_ASSIGN: return "cumass";
    case SUM: return "sum";
    case SUB: return "sub";
    case AND: return "and";
    case OR: return "or";
    case LESS: return "ls";
    case GREATER: return "gr";
    case TRUE: return "nocap";
    case FALSE: return "cap";
    case COMMA: return "com";
    case SEMICOLON: return "scol";
    case PARENTHESIS_OPEN: return "po";
    case PARENTHESIS_CLOSE: return "pc";
    case CURLY_BRACKET_OPEN: return "cbo";
    case CURLY_BRACKET_CLOSE: return "cbc";
    case CINT: return "cint";
    case CSTR: return "cstr";
    }

    assert(false);
    abort();
}

/**
 * Devuelve el valor del contenido de un token como texto.
 * @param token La variante con el contenido del token.
 * @return El valor del contenido en forma de texto.
 */
constexpr std::string TokenAttributeToString(const auto& token) {
    return std::visit(
        []<typename U>(U&& arg) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<U>, std::string>)
                return std::format("\"{}\"", EscapeUtf8String(arg));
            else if constexpr (std::is_same_v<std::decay_t<U>, std::monostate>)
                return "";
            else
                return std::to_string(arg);
        },
        token.attribute
    );
}
