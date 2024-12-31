#pragma once

#include <string>
#include <vector>

class StringProduct {
    std::vector<std::string> m_strings;

public:
    StringProduct() = default;
    explicit StringProduct(const std::string& str) : m_strings({ str }) {}

    [[nodiscard]] const auto& GetStrings() const { return m_strings; }

    void Append(std::string_view s) {
        m_strings.emplace_back(s);
    }

    void Append(const StringProduct& a) {
        const auto& newStrings = a.GetStrings();

        m_strings.reserve(m_strings.size() + newStrings.size());
        for (const auto& s : newStrings) {
            m_strings.emplace_back(s);
        }
    }

    StringProduct& operator=(std::string_view s) {
        m_strings.clear();
        m_strings.emplace_back(s);
        return *this;
    }
};

constexpr bool operator==(const StringProduct& a, const StringProduct& b) {
    return a.GetStrings() == b.GetStrings();
}

constexpr bool operator==(const StringProduct& a, std::string_view b) {
    const auto& strings = a.GetStrings();
    return strings.size() == 1 && strings[0] == b;
}

constexpr StringProduct operator*(const StringProduct& a, const StringProduct& b) {
    StringProduct result = a;
    result.Append(b);
    return result;
}

constexpr StringProduct operator*(const StringProduct& a, std::string_view b) {
    StringProduct result = a;
    result.Append(b);
    return result;
}
