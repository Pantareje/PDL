#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <ranges>

struct Symbol {
    std::string lex;
    std::map<std::string, std::string> attributes;
};

class SymbolTables {
    class Table {
        size_t m_tableId;
        std::vector<Symbol> m_symbols;
        std::map<std::string, size_t> m_nameMap;

    public:
        explicit Table(size_t tableId) : m_tableId(tableId) {}

        size_t AddSymbol(const std::string& name) {
            assert(!name.empty());
            assert(!m_nameMap.contains(name));

            size_t symbolPosition = m_symbols.size();
            m_symbols.emplace_back(name);
            m_nameMap.emplace(name, symbolPosition);

            return symbolPosition;
        }

        void AddAttribute(size_t pos, const std::string& name, const std::string& value) {
            assert(pos < m_symbols.size());
            Symbol& symbol = m_symbols[pos];

            assert(!name.empty());
            assert(!symbol.attributes.contains(name));
            symbol.attributes.emplace(name, value);
        }

        [[nodiscard]] std::optional<size_t> SearchSymbol(const std::string& name) const {
            if (!m_nameMap.contains(name))
                return std::nullopt;

            return m_nameMap.at(name);
        }

        void WriteTable(std::ostream& output) const {
            output << "#" << m_tableId << ":\n";
            for (const auto& symbol : m_symbols) {
                output << "*'" << symbol.lex << "'\n";
                for (const auto& [name, value] : symbol.attributes) {
                    output << "+" << name << ":" << value << "\n";
                }
            }
        }
    };

    uint32_t m_tableCounter = 0;
    std::vector<Table> m_tables;

public:
    SymbolTables() {
        CreateTable();
    }

    void CreateTable() {
        m_tables.emplace_back(m_tableCounter);
        m_tableCounter += 1;
    }

    void DestroyTable() {
        m_tables.pop_back();
    }

    size_t AddSymbol(const std::string& name) {
        return m_tables.back().AddSymbol(name);
    }

    size_t AddGlobalSymbol(const std::string& symbol) {
        return m_tables.front().AddSymbol(symbol);
    }

    void AddAttribute(size_t pos, const std::string& name, const std::string& value) {
        m_tables.back().AddAttribute(pos, name, value);
    }

    [[nodiscard]] std::optional<size_t> SearchSymbol(const std::string& name) const {
        for (const auto& table : std::ranges::views::reverse(m_tables)) {
            const auto result = table.SearchSymbol(name);
            if (result.has_value()) {
                return result;
            }
        }

        return std::nullopt;
    }

    void WriteTable(std::ostream& output) const {
        m_tables.back().WriteTable(output);
    }
};
