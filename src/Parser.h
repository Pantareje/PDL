#pragma once

#include "Lexer.h"
#include "SyntaxException.h"
#include "ValueProduct.h"

#include <sstream>

typedef std::map<std::string, ValueProduct> RuleAttributes;

class Parser {
    Lexer m_lexer;
    Token m_lastToken;

    [[noreturn]]
    void ThrowSyntaxError(SyntaxError error) const {
        throw SyntaxException(error, m_lastToken);
    }

    [[noreturn]]
    void ThrowSyntaxError(const char* errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }

    [[noreturn]]
    void ThrowSyntaxError(const std::string& errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }

    void LogSemanticError(GlobalState& globals, std::string_view message) const {
        globals.errorManager.LogSemanticError(m_lastToken.line, m_lastToken.column, message);
    }


    void GetNextToken(GlobalState& globals) {
        m_lastToken = m_lexer.GetToken(globals);
    }


    void VerifyTokenType(TokenType expectedType, const char* errorMessage) const {
        if (m_lastToken.type != expectedType)
            ThrowSyntaxError(errorMessage);
    }


    [[nodiscard]] RuleAttributes Axiom(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Function(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes FunType(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes VarType(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes FunAttributes(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes NextAttributes(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Body(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Statement(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes AtomStatement(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes IdAct(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ForAct(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Ass(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes CallParams(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes NextParams(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ReturnExp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Exp1(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ExpOr(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Exp2(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ExpAnd(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Exp3(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Comp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes CompOp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Exp4(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes Arith(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ArithOp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes ExpAtom(std::ostream& output, GlobalState& globals);

    [[nodiscard]] RuleAttributes IdVal(std::ostream& output, GlobalState& globals);

public:
    explicit Parser(std::istream& input) : m_lexer(input), m_lastToken() {}

    void Parse(std::ostream& output, GlobalState& globals);
};
