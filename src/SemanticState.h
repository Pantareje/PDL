#pragma once

struct SemanticState {
    bool useSemantic = true;

    std::optional<SymbolTable> globalTable;
    std::optional<SymbolTable> localTable;

    uint32_t tableCounter{};

    size_t globalOffset{};
    size_t localOffset{};

    bool implicitDeclaration{};
};
