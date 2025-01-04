#pragma once

#include "language/core/Symbol.h"

#include <cassert>
#include <vector>
#include <map>
#include <string>


constexpr char aType[] = "type";
constexpr char aRetType[] = "ret_type";
constexpr char aWidth[] = "width";
constexpr char aSum[] = "sum";
constexpr char aFunCall[] = "funCall";

constexpr char tOk[] = "type_ok";
constexpr char tError[] = "type_error";

constexpr char tVoid[] = "void";
constexpr char tFun[] = "function";
constexpr char tInt[] = "int";
constexpr char tLog[] = "boolean";
constexpr char tStr[] = "string";

constexpr int16_t wInt = 2;
constexpr int16_t wLog = 1;
constexpr int16_t wStr = 64;


class SymbolTable {
    uint64_t m_tableId;
    std::vector<Symbol> m_symbols;
    std::map<std::string, uint64_t> m_nameMap;

public:
    explicit SymbolTable(uint64_t tableId) : m_tableId(tableId) {}

    uint64_t AddSymbol(const std::string& name) {
        assert(!name.empty());
        assert(!m_nameMap.contains(name));

        uint64_t symbolPosition = m_symbols.size();
        m_symbols.emplace_back(name);
        m_nameMap.emplace(name, symbolPosition);

        return symbolPosition;
    }

    [[nodiscard]] std::string_view GetSymbolName(uint64_t pos) const {
        assert(m_symbols.size() > pos);
        return m_symbols[pos].lex;
    }

    void AddAttribute(uint64_t pos, std::string_view name, std::string_view value) {
        assert(pos < m_symbols.size());
        Symbol& symbol = m_symbols[pos];

        assert(!name.empty());
        assert(!symbol.attributes.contains(std::string(name)));
        symbol.attributes.emplace(name, value);
    }

    [[nodiscard]] bool HasAttribute(uint64_t pos, const std::string& name) const {
        assert(pos < m_symbols.size());
        const Symbol& symbol = m_symbols[pos];

        assert(!name.empty());
        return symbol.attributes.contains(name);
    }

    [[nodiscard]] const std::string& GetAttribute(uint64_t pos, const std::string& name) const {
        assert(pos < m_symbols.size());
        const Symbol& symbol = m_symbols[pos];

        assert(!name.empty());
        return symbol.attributes.at(name);
    }

    [[nodiscard]] std::optional<uint64_t> SearchSymbol(const std::string& name) const {
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
