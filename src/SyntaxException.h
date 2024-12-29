#pragma once

#include <stdexcept>

enum class SyntaxError {



};

constexpr const char* GetErrorMessage(SyntaxError error) {
    using enum SyntaxError;

    switch (error) {
    default:
        return "";
    }
}

class SyntaxException final : public std::runtime_error {
    SyntaxError m_error;
    unsigned m_lineNumber;
    unsigned m_columnNumber;

public:
    [[deprecated]]
    SyntaxException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const std::string& message
    ) noexcept
        : runtime_error(message),
          m_error(),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    [[deprecated]]
    SyntaxException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const char* message
    ) noexcept
        : runtime_error(message),
          m_error(),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    SyntaxException(
        const unsigned lineNumber,
        const unsigned columnNumber,
        const SyntaxError error
    ) noexcept
        : runtime_error(GetErrorMessage(error)),
          m_error(error),
          m_lineNumber(lineNumber),
          m_columnNumber(columnNumber) {}

    [[nodiscard]] SyntaxError GetError() const { return m_error; }
    [[nodiscard]] unsigned GetLine() const { return m_lineNumber; }
    [[nodiscard]] unsigned GetColumn() const { return m_columnNumber; }
};
