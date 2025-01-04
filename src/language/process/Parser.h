#pragma once

#include "language/process/Lexer.h"
#include "language/errors/SyntaxError.h"
#include "language/core/ValueProduct.h"

#include <sstream>

class Parser {
    struct Attributes : std::map<std::string, ValueProduct> {
        unsigned startLine;
        unsigned startColumn;
        unsigned endLine;
        unsigned endColumn;

        Attributes(
            unsigned startLine,
            unsigned startColumn,
            unsigned endLine,
            unsigned endColumn
        ) : startLine(startLine),
            startColumn(startColumn),
            endLine(endLine),
            endColumn(endColumn) {}
    };

    struct AttributesPosRef {
        size_t pos;
        std::vector<Attributes>* vec;

        operator const Attributes&() const {
            return (*vec)[pos];
        }

        ValueProduct& operator[](const std::string& key) const {
            return (*vec)[pos][key];
        }

        ValueProduct& operator[](std::string&& key) const {
            return (*vec)[pos][key];
        }

        const ValueProduct& at(const std::string& key) const {
            return (*vec)[pos].at(key);
        }
    };

    Lexer m_lexer;

    Token m_currentToken;

    std::vector<Attributes> m_attributesStack;

    [[noreturn]]
    void ThrowSyntaxError(SyntaxError error) const {
        throw SyntaxException(error, m_currentToken);
    }

    void LogSemanticError(
        GlobalState& globals,
        unsigned startLine,
        unsigned startColumn,
        unsigned endColumn,
        SemanticError error,
        std::string_view message
    ) const {
        if (m_currentToken.line == startLine) {
            assert(endColumn >= startColumn);
            globals.errorManager.LogSemanticError(
                m_lexer.GetCurrentLine(),
                startLine, startColumn,
                endColumn - startColumn,
                error, message
            );
        } else {
            globals.errorManager.LogSemanticError(
                m_lexer.GetCurrentLine(),
                startLine, startColumn,
                0,
                error, message
            );
        }
    }

    void LogSemanticError(
        GlobalState& globals,
        const Attributes& attributes,
        SemanticError error,
        std::string_view message
    ) const {
        LogSemanticError(
            globals,
            attributes.startLine,
            attributes.startColumn,
            attributes.endColumn,
            error, message
        );
    }


    void LogSemanticError(
        GlobalState& globals,
        const Attributes& startAttributes,
        const Attributes& endAttributes,
        SemanticError error,
        std::string_view message
    ) const {
        LogSemanticError(
            globals,
            startAttributes.startLine,
            startAttributes.startColumn,
            endAttributes.endColumn,
            error, message
        );
    }

    void LogSemanticError(
        GlobalState& globals,
        const Token& token,
        SemanticError error,
        std::string_view message
    ) const {
        LogSemanticError(
            globals,
            token.line,
            token.column,
            token.column + token.length,
            error, message
        );
    }

    [[deprecated]]
    void LogSemanticError(GlobalState& globals, std::string_view message) const noexcept {
        LogSemanticError(
            globals,
            m_currentToken,
            {},
            message
        );
    }


    AttributesPosRef CreateRuleAttributes() {
        m_attributesStack.emplace_back(
            m_currentToken.line,
            m_currentToken.column,
            m_currentToken.line,
            m_currentToken.column + m_currentToken.length
        );
        return { m_attributesStack.size() - 1, &m_attributesStack };
    }

    Attributes PopCurrentAttributes() {
        Attributes attributes = std::move(m_attributesStack.back());
        m_attributesStack.pop_back();
        return attributes;
    }


    void GetNextToken(GlobalState& globals) {
        for (Attributes& attributes : m_attributesStack) {
            attributes.endLine = m_currentToken.line;
            attributes.endColumn = m_currentToken.column + m_currentToken.length;
        }

        m_currentToken = m_lexer.GetToken(globals);
    }

    void VerifyTokenType(TokenType expectedType, SyntaxError error) const {
        if (m_currentToken.type != expectedType)
            ThrowSyntaxError(error);
    }


    [[nodiscard]] Attributes Axiom(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Function(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes FunType(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes VarType(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes FunAttributes(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes NextAttributes(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Body(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Statement(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes AtomStatement(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes IdAct(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ForAct(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Ass(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes CallParams(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes NextParams(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ReturnExp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Exp1(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ExpOr(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Exp2(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ExpAnd(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Exp3(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Comp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes CompOp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Exp4(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes Arith(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ArithOp(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes ExpAtom(std::ostream& output, GlobalState& globals);

    [[nodiscard]] Attributes IdVal(std::ostream& output, GlobalState& globals);

public:
    explicit Parser(std::istream& input) : m_lexer(input), m_currentToken() {}

    void Parse(std::ostream& output, GlobalState& globals);
};
