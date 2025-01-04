#pragma once

#include "Token.h"

#include <stdexcept>
#include <utility>

enum class SyntaxError {};

constexpr const char* GetErrorMessage(SyntaxError error) {
    using enum SyntaxError;

    switch (error) {
    default:
        return "";
    }
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
        Token token
    ) noexcept
        : runtime_error(GetErrorMessage(error)),
          m_error(error),
          m_token(std::move(token)) {}

    [[nodiscard]] SyntaxError GetCode() const { return m_error; }
    [[nodiscard]] unsigned GetLine() const { return m_token.line; }
    [[nodiscard]] unsigned GetColumn() const { return m_token.column; }
};
