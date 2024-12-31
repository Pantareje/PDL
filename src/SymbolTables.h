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


class SymbolTable {
    size_t m_tableId;
    std::vector<Symbol> m_symbols;
    std::map<std::string, size_t> m_nameMap;

public:
    explicit SymbolTable(size_t tableId) : m_tableId(tableId) {}

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

[[deprecated]]
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

    uint32_t m_tableCounter = 1;

    Table m_globalTable = Table(0);
    std::optional<Table> m_localTable = std::nullopt;

    Table& GetCurrentTable() {
        return m_localTable.has_value() ? m_localTable.value() : m_globalTable;
    }

public:
    void CreateTable() {
        m_localTable = Table(m_tableCounter);
        m_tableCounter += 1;
    }

    void DestroyTable() {
        m_localTable = std::nullopt;
    }

    size_t AddSymbol(const std::string& name) {
        Table& table = GetCurrentTable();
        return table.AddSymbol(name);
    }

    size_t AddGlobalSymbol(const std::string& name) {
        return m_globalTable.AddSymbol(name);
    }

    void AddAttribute(size_t pos, const std::string& name, const std::string& value) {
        Table& table = GetCurrentTable();
        table.AddAttribute(pos, name, value);
    }

    [[nodiscard]] std::optional<size_t> SearchSymbol(const std::string& name) const {
        if (m_localTable.has_value()) {
            const auto result = m_localTable->SearchSymbol(name);
            if (result.has_value())
                return result;
        }

        return m_globalTable.SearchSymbol(name);
    }

    void WriteLocalTable(std::ostream& output) const {
        assert(m_localTable.has_value());
        m_localTable->WriteTable(output);
    }

    void WriteGlobalTable(std::ostream& output) const {
        m_globalTable.WriteTable(output);
    }
};
