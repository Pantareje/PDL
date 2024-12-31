#pragma once

#include "Lexer.h"
#include "SyntaxException.h"
#include "StringProduct.h"

#include <sstream>

typedef std::map<std::string, StringProduct> RuleAttributes;

class Parser {
    using enum TokenType;

    Lexer m_lexer;
    Token m_lastToken;

    void ThrowError(SyntaxError error) const {
        throw SyntaxException(error, m_lastToken);
    }

    [[noreturn]]
    void ThrowError(const char* errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }

    [[noreturn]]
    void ThrowError(const std::string& errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }


    void VerifyTokenType(TokenType expectedType, const char* errorMessage) const {
        if (m_lastToken.type != expectedType)
            ThrowError(errorMessage);
    }

    void VerifyTokenType(TokenType expectedType, SyntaxError error) const {
        if (m_lastToken.type != expectedType)
            ThrowError(error);
    }


    [[nodiscard]] RuleAttributes Axiom(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Function(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes FunType(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes VarType(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes FunAttributes(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes NextAttribute(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Body(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Statement(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes AtomStatement(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes IdAct(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ForAct(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Ass(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes CallParam(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes NextParam(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ReturnExp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Exp1(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ExpOr(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Exp2(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ExpAnd(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Exp3(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Comp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes CompOp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Exp4(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes Arith(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ArithOp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes ExpAtom(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

    [[nodiscard]] RuleAttributes IdVal(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);

public:
    explicit Parser(std::istream& input) : m_lexer(input), m_lastToken() {}

    void Parse(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals);
};
