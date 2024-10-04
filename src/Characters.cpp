// Para una explicación detallada sobre las codificaciones unicode:
// https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G7404#include "Characters.h"

#include "Characters.h"

#include <cassert>
#include <sstream>

namespace {
    typedef unsigned char uchar;

    template<size_t count>
    constexpr char32_t AppendUnicodeBytes(const char32_t source, std::istream& input) {
        char32_t codepoint = source;

        for (size_t i = 0; i < count; i++) {
            const int c = input.get();
            if (c == EOF || (c & ~0x3F) != 0x80) return U'\uFFFD';

            codepoint = codepoint << 6 | static_cast<uchar>(c) & 0x3F;
        }

        return codepoint;
    }

    constexpr char CharCast(const char32_t codepoint) {
        assert(codepoint < 0xFF);
        return static_cast<char>(static_cast<unsigned char>(codepoint));
    }
}

char32_t ReadUtf8(std::istream& input) {
    const int readChar = input.get();
    if (readChar == EOF) return static_cast<char32_t>(EOF);
    char32_t codepoint;

    // Carácter ASCII.
    if ((readChar & ~0x7F) == 0x00) {
        codepoint = static_cast<uchar>(readChar) & 0x7F;
    }
    // Carácter de dos bytes.
    else if ((readChar & ~0x1F) == 0xC0) {
        // Los caracteres de dos bytes que empiezan
        // por 0xC0 o 0xC1 no son válidos.
        if (readChar == 0xC0 || readChar == 0xC1) {
            codepoint = U'\uFFFD';
        } else {
            codepoint = static_cast<uchar>(readChar) & 0x1F;
            codepoint = AppendUnicodeBytes<1>(codepoint, input);
        }
    }
    // Carácter de tres bytes.
    else if ((readChar & ~0x0F) == 0xE0) {
        codepoint = static_cast<uchar>(readChar) & 0x0F;
        codepoint = AppendUnicodeBytes<2>(codepoint, input);
    }
    // Carácter de cuatro bytes.
    else if ((readChar & ~0x07) == 0xF0) {
        codepoint = static_cast<uchar>(readChar) & 0x07;
        codepoint = AppendUnicodeBytes<3>(codepoint, input);

        // Cuando procesamos 4 bytes, nos aseguramos de
        // que no se exceda el valor máximo.
        if (codepoint > static_cast<char32_t>(0x10FFFF))
            codepoint = U'\uFFFD';
    }
    // Carácter inválido.
    else {
        codepoint = U'\uFFFD';
    }

    return codepoint;
}

std::string ToUtf8(const char32_t codepoint) {
    std::string result;

    // Carácter ASCII.
    if (codepoint < 0x80) {
        result += CharCast(codepoint);
    }
    // Carácter de 2 bytes.
    else if (codepoint < 0x800) {
        result += CharCast((codepoint >> 6 & 0x1F) | (0xC0));
        result += CharCast((codepoint >> 0 & 0x3F) | (0x80));
    }
    // Carácter de 3 bytes.
    else if (codepoint < 0x10000) {
        result += CharCast((codepoint >> 12 & 0x0F) | (0xE0));
        result += CharCast((codepoint >>  6 & 0x3F) | (0x80));
        result += CharCast((codepoint >>  0 & 0x3F) | (0x80));
    }
    // Carácter de 4 bytes.
    else if (codepoint < 0x10FFFF) {
        result += CharCast((codepoint >> 18 & 0x07) | (0xF0));
        result += CharCast((codepoint >> 12 & 0x3F) | (0x80));
        result += CharCast((codepoint >>  6 & 0x3F) | (0x80));
        result += CharCast((codepoint >>  0 & 0x3F) | (0x80));
    }
    // Carácter inválido.
    else {
        result += "\uFFFD";
    }

    return result;
}

size_t CountCodepointsUtf8(const std::string& str) {
    std::istringstream iss(str);
    size_t count = 0;

    while (ReadUtf8(iss) != static_cast<char32_t>(EOF))
        count += 1;

    return count;
}
