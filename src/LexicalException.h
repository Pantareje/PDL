#pragma once

#include <stdexcept>

class LexicalException : public std::runtime_error {
    unsigned m_lineNumber;
    unsigned m_columnNumber;

public:
    explicit LexicalException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const std::string& message
    ) noexcept
        : runtime_error(message),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    explicit LexicalException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const char* message
    ) noexcept
        : runtime_error(message),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    [[nodiscard]] unsigned GetLine() const { return m_lineNumber; }
    [[nodiscard]] unsigned GetColumn() const { return m_columnNumber; }
};
