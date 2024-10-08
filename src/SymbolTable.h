#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <optional>

struct Symbol {
    std::string lex;
};

class SymbolTable {
    class SubTable {
        std::vector<Symbol> m_symbols;
        std::map<std::string, size_t> m_nameMap;

    public:
        size_t AddSymbol(const std::string& symbol) {
            assert(!symbol.empty());
            assert(!m_nameMap.contains(symbol));

            size_t insertionIndex = m_symbols.size();
            m_nameMap.emplace(symbol, insertionIndex);
            m_symbols.emplace_back(symbol);

            return insertionIndex;
        }

        [[nodiscard]] std::optional<size_t> SearchSymbol(const std::string& name) {
            if (!m_nameMap.contains(name))
                return std::nullopt;

            return m_nameMap[name];
        }

        void WriteSymbols(std::ostream& output) const {
            for (const auto& symbol : m_symbols) {
                output << "*'" << symbol.lex << "'\n";
            }
        }
    };

    SubTable m_table;

public:
    [[nodiscard]] std::optional<size_t> SearchSymbol(const std::string& name) {
        return m_table.SearchSymbol(name);
    }

    size_t AddSymbol(const std::string& symbol) {
        return m_table.AddSymbol(symbol);
    }

    // TODO: Convertirlo en una función para subtablas al añadir múltiples.
    void WriteTable(std::ostream& output) const {
        output << "Tabla Global #" << 0 << ":\n";
        m_table.WriteSymbols(output);
    }
};