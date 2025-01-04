#pragma once

#include <string>
#include <vector>
#include <variant>

class ValueProduct {
    std::variant<std::monostate, std::vector<std::string>, std::vector<int16_t>> m_data;

public:
    ValueProduct() = default;

    explicit ValueProduct(const std::string& str) : m_data(std::vector { str }) {}
    explicit ValueProduct(int16_t value) : m_data(std::vector { value }) {}

    [[nodiscard]] const auto& GetRawData() const { return m_data; }

    [[nodiscard]] const auto& GetStrings() const { return std::get<std::vector<std::string>>(m_data); }

    [[nodiscard]] const auto& GetNumbers() const { return std::get<std::vector<int16_t>>(m_data); }

    void Append(std::string_view s) {
        if (std::holds_alternative<std::monostate>(m_data)) {
            std::vector<std::string> data;
            data.emplace_back(s);
            m_data = data;
        } else if (std::holds_alternative<std::vector<std::string>>(m_data)) {
            auto& data = std::get<std::vector<std::string>>(m_data);
            data.emplace_back(s);
        }
    }

    void Append(const ValueProduct& a) {
        if (std::holds_alternative<std::vector<std::string>>(m_data)) {
            auto& data = std::get<std::vector<std::string>>(m_data);
            const auto& newData = std::get<std::vector<std::string>>(a.m_data);

            data.reserve(data.size() + newData.size());
            for (const auto& s : newData) {
                data.emplace_back(s);
            }
        } else if (std::holds_alternative<std::vector<int16_t>>(m_data)) {
            auto& data = std::get<std::vector<int16_t>>(m_data);
            const auto& newData = std::get<std::vector<int16_t>>(a.m_data);

            data.reserve(data.size() + newData.size());
            for (const auto& s : newData) {
                data.emplace_back(s);
            }
        }
    }

    ValueProduct& operator=(std::string_view s) {
        if (std::holds_alternative<std::monostate>(m_data)) {
            std::vector<std::string> data;
            data.emplace_back(s);
            m_data = data;
        } else if (std::holds_alternative<std::vector<std::string>>(m_data)) {
            auto& data = std::get<std::vector<std::string>>(m_data);
            data.clear();
            data.emplace_back(s);
        } else {
            m_data = std::vector { std::string(s) };
        }

        return *this;
    }

    ValueProduct& operator=(int16_t a) {
        if (std::holds_alternative<std::monostate>(m_data)) {
            std::vector<int16_t> data;
            data.emplace_back(a);
            m_data = data;
        } else if (std::holds_alternative<std::vector<int16_t>>(m_data)) {
            auto& data = std::get<std::vector<int16_t>>(m_data);
            data.clear();
            data.emplace_back(a);
        } else {
            m_data = std::vector { a };
        }

        return *this;
    }

    operator int16_t() const {
        const auto& data = std::get<std::vector<int16_t>>(m_data);
        assert(data.size() == 1);
        return data[0];
    }

    operator std::string_view() const {
        const auto& data = std::get<std::vector<std::string>>(m_data);
        assert(data.size() == 1);
        return data[0];
    }
};

constexpr bool operator==(const ValueProduct& a, const ValueProduct& b) {
    return a.GetRawData() == b.GetRawData();
}

constexpr bool operator==(const ValueProduct& a, std::string_view b) {
    const auto& strings = a.GetStrings();
    return strings.size() == 1 && strings[0] == b;
}

constexpr bool operator==(const ValueProduct& a, int16_t b) {
    const auto& numbers = a.GetNumbers();
    return numbers.size() == 1 && numbers[0] == b;
}

constexpr ValueProduct operator*(const ValueProduct& a, const ValueProduct& b) {
    ValueProduct result { a };
    result.Append(b);
    return result;
}

constexpr ValueProduct operator*(const ValueProduct& a, std::string_view b) {
    ValueProduct result { a };
    result.Append(b);
    return result;
}

constexpr ValueProduct operator*(std::string_view a, const ValueProduct& b) {
    ValueProduct result { std::string(a) };
    result.Append(b);
    return result;
}
