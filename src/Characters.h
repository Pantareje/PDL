#pragma once

#include <unicode/uchar.h>

#include <string>
#include <istream>

constexpr bool IsAscii(const char32_t c) {
    return (c & ~0x7F) == 0;
}

constexpr bool IsPrintAscii(const char32_t c) {
    return IsAscii(c) && std::isprint(static_cast<unsigned char>(c));
}

constexpr bool IsSpaceAscii(const char32_t c) {
    return IsAscii(c) && std::isspace(static_cast<unsigned char>(c));
}

constexpr bool IsGraphAscii(const char32_t c) {
    return IsAscii(c) && std::isgraph(static_cast<unsigned char>(c));
}

constexpr bool IsAlphaAscii(const char32_t c) {
    return IsAscii(c) && std::isalpha(static_cast<unsigned char>(c));
}

constexpr bool IsAlphaUnicode(const char32_t c) {
    return u_isalpha(static_cast<UChar32>(c));
}

constexpr bool IsAlnumAscii(const char32_t c) {
    return IsAscii(c) && std::isalnum(static_cast<unsigned char>(c));
}

constexpr bool IsAlnumUnicode(const char32_t c) {
    return u_isalnum(static_cast<UChar32>(c));
}

constexpr bool IsDigitAscii(const char32_t c) {
    return IsAscii(c) && std::isdigit(static_cast<unsigned char>(c));
}

constexpr bool IsPrintUnicode(const char32_t c) {
    return u_isprint(static_cast<UChar32>(c));
}


/**
 * Convierte el carácter ASCII escapado tras la
 * barra torcida su carácter adecuado.
 * @param c El carácter que representa el valor a escapar.
 * @return El valor escapado. -1 si no representa un escapado conocido.
 */
constexpr int EscapedToAscii(const char32_t c) {
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

/**
 * Escapa el carácter si es necesario.
 * @param c El carácter ASCII a escapar.
 * @return La secuencia escapada.
 */
constexpr std::string EscapeAsciiChar(const char c) {
    switch (c) {
    case '\\': return "\\\\";
    case '\'': return "\\\'";

    case '\0': return "\\0";

    case '\n': return "\\n";
    case '\a': return "\\a";
    case '\t': return "\\t";
    case '\v': return "\\v";
    case '\f': return "\\f";
    case '\r': return "\\r";

    default: return { c };
    }
}

/**
 * Escapa todos los caracteres especiales ASCII de una cadena.
 * @param str La cadena a escapar.
 * @return La cadena escapada.
 */
constexpr std::string EscapeUtf8String(const std::string_view str) {
    std::string result;

    for (const char& c : str) {
        if (!IsAscii(c)) {
            result += c;
        } else {
            result += EscapeAsciiChar(c);
        }
    }

    return result;
}

/**
 * Retorna el siguiente punto de código Unicode de un flujo UTF-8.
 * @param input Un flujo de entrada codificado en UTF-8.
 * @return El siguiente punto de código, en formato UTF-32.
 */
char32_t ReadUtf8Codepoint(std::istream& input);

/**
 * Convierte un punto unicode de UTF-32 a UTF-8.
 * @param codepoint El punto Unicode a representar en UTF-8.
 * @return Una cadena que representa el punto en UTF-8.
 */
std::string CodepointToUtf8(char32_t codepoint);
