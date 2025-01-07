#pragma once

#include "language/errors/ErrorManager.h"
#include "language/core/ValueProduct.h"
#include "language/core/SymbolTable.h"

#include <cstdio>

enum class SyntaxPrint {
    NONE, PARSE, SYMBOLS
};

struct GlobalState {
    static constexpr char TYPE_KEY[] = "tipo";
    static constexpr char OFFSET_KEY[] = "despl";
    static constexpr char FUN_TAG_KEY[] = "etiqFuncion";
    static constexpr char RET_TYPE_KEY[] = "tipoRetorno";
    static constexpr char PARAM_COUNT_KEY[] = "numParam";

    static constexpr char PARAM_TYPE_PKEY[] = "tipoParam";

    ErrorManager errorManager;

    SyntaxPrint syntaxPrint = SyntaxPrint::NONE;
    bool useSemantic = true;

    std::optional<SymbolTable> globalTable;
    std::optional<SymbolTable> localTable;

    uint32_t tableCounter {};

    size_t globalOffset {};
    size_t localOffset {};

    bool implicitDeclaration {};

    size_t& CurrentOffset() { return localTable.has_value() ? localOffset : globalOffset; }

    [[nodiscard]] constexpr std::optional<SymbolPos> SearchSymbol(const std::string& name) const {
        if (localTable.has_value()) {
            const auto result = localTable->SearchSymbol(name);
            if (result.has_value()) {
                return SymbolPos {
                    .isLocal = true,
                    .pos = result.value()
                };
            }
        }

        const auto result = globalTable->SearchSymbol(name);
        if (result.has_value()) {
            return SymbolPos {
                .isLocal = false,
                .pos = result.value()
            };
        }

        return std::nullopt;
    }

    SymbolPos AddSymbol(const std::string& name) {
        return localTable.has_value()
                   ? SymbolPos { .isLocal = true, .pos = localTable->AddSymbol(name) }
                   : SymbolPos { .isLocal = false, .pos = globalTable->AddSymbol(name) };
    }

    SymbolPos AddGlobalSymbol(const std::string& name) {
        return SymbolPos { .isLocal = false, .pos = globalTable->AddSymbol(name) };
    }

    [[nodiscard]] bool HasAttribute(SymbolPos pos, const std::string& name) const {
        if (pos.isLocal) {
            assert(localTable.has_value());
            return localTable->HasAttribute(pos.pos, name);
        }

        assert(globalTable.has_value());
        return globalTable->HasAttribute(pos.pos, name);
    }

    void AddRawAttribute(SymbolPos pos, std::string_view key, std::string_view value) {
        if (pos.isLocal) {
            assert(localTable.has_value());
            localTable->AddAttribute(pos.pos, key, value);
        } else {
            assert(globalTable.has_value());
            globalTable->AddAttribute(pos.pos, key, value);
        }
    }

    void AddStringAttribute(SymbolPos pos, std::string_view key, std::string_view value) {
        AddRawAttribute(pos, key, std::format("'{}'", value));
    }

    void AddSizeAttribute(SymbolPos pos, std::string_view key, size_t value) {
        AddRawAttribute(pos, key, std::to_string(value));
    }

    [[nodiscard]] std::string_view GetRawAttribute(SymbolPos pos, const std::string& name) const {
        if (pos.isLocal) {
            assert(localTable.has_value());
            return localTable->GetAttribute(pos.pos, name);
        }

        assert(globalTable.has_value());
        return globalTable->GetAttribute(pos.pos, name);
    }

    [[nodiscard]] std::string_view GetStringAttribute(SymbolPos pos, const std::string& name) const {
        const std::string_view result = GetRawAttribute(pos, name);
        assert(result.size() >= 2);
        return result.substr(1, result.size() - 2);
    }

    [[nodiscard]] size_t GetSizeAttribute(SymbolPos pos, const std::string& name) const {
        const auto raw = GetRawAttribute(pos, name);
        size_t value;
        const auto res = std::sscanf(std::string(raw).c_str(), "%zd", &value);
        assert(res == 1);
        return value;
    }

    void AddType(SymbolPos pos, const ValueProduct& type) {
        AddStringAttribute(pos, TYPE_KEY, type);
    }

    void AddOffset(SymbolPos pos, size_t offset) {
        AddSizeAttribute(pos, OFFSET_KEY, offset);
    }

    void AddFunctionType(SymbolPos pos, const ValueProduct& retType, const ValueProduct& paramType) {
        const auto& paramValues = paramType.GetStrings();

        assert(!paramValues.empty());

        AddStringAttribute(pos, TYPE_KEY, tFun);
        AddStringAttribute(pos, RET_TYPE_KEY, retType);

        if (paramValues[0] == tVoid) {
            AddSizeAttribute(pos, PARAM_COUNT_KEY, 0);
        } else {
            AddSizeAttribute(pos, PARAM_COUNT_KEY, paramValues.size());

            for (size_t i = 0; i < paramValues.size(); i++) {
                AddStringAttribute(pos, PARAM_TYPE_PKEY + std::format("{:02}", i + 1), paramValues[i]);
            }
        }
    }

    void AddFunctionTag(SymbolPos pos) {
        assert(!pos.isLocal);
        AddStringAttribute(pos, FUN_TAG_KEY, globalTable->GetSymbolName(pos.pos));
    }

    [[nodiscard]] std::string_view GetSymbolName(SymbolPos pos) const {
        if (pos.isLocal) {
            assert(localTable.has_value());
            return localTable->GetSymbolName(pos.pos);
        }

        assert(globalTable.has_value());
        return globalTable->GetSymbolName(pos.pos);
    }

    [[nodiscard]] bool HasType(SymbolPos pos) const {
        return HasAttribute(pos, TYPE_KEY);
    }

    [[nodiscard]] ValueProduct GetType(SymbolPos pos) const {
        ValueProduct result = {};

        const auto type = GetStringAttribute(pos, TYPE_KEY);
        result.Append(type);

        if (type == tFun) {
            result.Append(GetStringAttribute(pos, RET_TYPE_KEY));
            const size_t paramCount = GetSizeAttribute(pos, PARAM_COUNT_KEY);
            for (size_t i = 0; i < paramCount; i++) {
                result.Append(GetStringAttribute(pos, PARAM_TYPE_PKEY + std::format("{:02}", i + 1)));
            }
        }

        return result;
    }
};
