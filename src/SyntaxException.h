#pragma once

#include <stdexcept>

class SyntaxException final : public std::runtime_error
{
public:
    SyntaxException(const std::string& message) noexcept : runtime_error(message) {}
};
