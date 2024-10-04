#pragma once

#include <string>
#include <istream>

constexpr bool IsAscii(const char32_t c) {
    return (c & 0x80) == 0;
}

constexpr bool IsPrint(const char32_t c) {
    return IsAscii(c) && std::isprint(static_cast<unsigned char>(c));
}

constexpr bool IsSpace(const char32_t c) {
    return IsAscii(c) && std::isspace(static_cast<unsigned char>(c));
}

constexpr bool IsGraph(const char32_t c) {
    return IsAscii(c) && std::isgraph(static_cast<unsigned char>(c));
}

constexpr bool IsAlpha(const char32_t c) {
    return IsAscii(c) && std::isalpha(static_cast<unsigned char>(c));
}

constexpr bool IsAlnum(const char32_t c) {
    return IsAscii(c) && std::isalnum(static_cast<unsigned char>(c));
}

constexpr bool IsDigit(const char32_t c) {
    return IsAscii(c) && std::isdigit(static_cast<unsigned char>(c));
}

constexpr bool IsSource(const char32_t c) {
    if (c == static_cast<char32_t>(EOF)) return false;
    return !IsAscii(c) || IsSpace(c) || IsGraph(c);
}

/**
 * Retorna el siguiente punto de código Unicode de un flujo UTF-8.
 * @param input Un flujo de entrada codificado en UTF-8.
 * @return El siguiente punto de código, en formato UTF-32.
 */
char32_t ReadUtf8(std::istream& input);

/**
 * Convierte un punto unicode de UTF-32 a UTF-8.
 * @param codepoint El punto Unicode a representar en UTF-8.
 * @return Una cadena que representa el punto en UTF-8.
 */
std::string ToUtf8(char32_t codepoint);

/**
 * Cuenta el número de puntos Unicode en una cadena UTF-8.
 * @param str La cadena UTF-8 a evaluar.
 * @return El número de puntos Unicode en la cadena.
 */
size_t CountCodepointsUtf8(const std::string& str);
