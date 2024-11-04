#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "Lexer.h"
#include "SyntaxException.h"

#include <sstream>
#include <iostream>

// TODO: Actualizar los mensajes de error.

class Parser {
    using
    enum TokenType;

    Lexer m_lexer;
    Token m_nextToken;

    //
    void Axiom(std::ostream& output, SymbolTable& symbolTable) {
        // P -> FUNCTION P | STATEMENT P | lambda
        switch (m_nextToken.type) {
            // First (FUNCTION P)
            case FUNCTION:
                output << "1 ";
                Function(output, symbolTable);
                Axiom(output, symbolTable);
                break;

                // First (STATEMENT P)
            case IF:
            case FOR:
            case VAR:
            case IDENTIFIER:
            case OUTPUT:
            case INPUT:
            case RETURN:
                output << "2 ";
                Statement(output, symbolTable);
                Axiom(output, symbolTable);
                break;

                // Como P -> lambda, Follow (P)
            case END:
                output << "3 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Function(std::ostream& output, SymbolTable& symbolTable) {
        // FUNCTION -> function FUNTYPE id ( FUNATTRIBUTES ) { BODY }

        output << "4 ";

        assert(m_nextToken.type == FUNCTION);
        m_nextToken = m_lexer.GetToken(symbolTable);

        FunType(output, symbolTable);

        if (m_nextToken.type != TokenType::IDENTIFIER)
            throw SyntaxException("");
        m_nextToken = m_lexer.GetToken(symbolTable);

        if (m_nextToken.type != TokenType::PARENTHESIS_OPEN)
            throw SyntaxException("");
        m_nextToken = m_lexer.GetToken(symbolTable);

        FunAttributes(output, symbolTable);

        if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
            throw SyntaxException("");
        m_nextToken = m_lexer.GetToken(symbolTable);

        if (m_nextToken.type != TokenType::CURLY_BRACKET_OPEN)
            throw SyntaxException("");
        m_nextToken = m_lexer.GetToken(symbolTable);

        Body(output, symbolTable);

        if (m_nextToken.type != TokenType::CURLY_BRACKET_CLOSE)
            throw SyntaxException("");
        m_nextToken = m_lexer.GetToken(symbolTable);
    }

    //
    void FunType(std::ostream& output, SymbolTable& symbolTable) {
        // FUNTYPE -> void | VARTYPE

        switch (m_nextToken.type) {
            // First (void)
            case VOID:
                output << "5 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (VARTYPE)
            case INT:
            case BOOL:
            case STRING:
                output << "6 ";

                VarType(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void VarType(std::ostream& output, SymbolTable& symbolTable) {
        // VARTYPE -> int | boolean | string
        switch (m_nextToken.type) {
            // First (int)
            case INT:
                output << "7 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (bool)
            case BOOL:
                output << "8 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (string)
            case STRING:
                output << "9 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void FunAttributes(std::ostream& output, SymbolTable& symbolTable) {
        // FUNATTRIBUTES -> void | VARTYPE id NEXTATTRIBUTE
        switch (m_nextToken.type) {
            // First (void)
            case VOID:
                output << "10 ";
                break;

                // First (VARTYPE id NEXTATTRIBUTE)
            case INT:
            case BOOL:
            case STRING:
                output << "11 ";
                VarType(output, symbolTable);

                if (m_nextToken.type != IDENTIFIER)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                NextAttribute(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void NextAttribute(std::ostream& output, SymbolTable& symbolTable) {
        // NEXTATTRIBUTE -> , VARTYPE id NEXTATTRIBUTE | lambda
        switch (m_nextToken.type) {
            // First (, VARTYPE id NEXTATTRIBUTE)
            case COMMA:
                output << "12 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                VarType(output, symbolTable);

                if (m_nextToken.type != IDENTIFIER)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                NextAttribute(output, symbolTable);
                break;

                // Como NEXTATTRIBUTE -> lambda, Follow (NEXTATTRIBUTE)
            case PARENTHESIS_CLOSE:
                output << "13 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Body(std::ostream& output, SymbolTable& symbolTable) {
        // BODY -> STATEMENT BODY | lambda
        switch (m_nextToken.type) {
            // First (STATEMENT BODY)
            case IF:
            case FOR:
            case VAR:
            case IDENTIFIER:
            case OUTPUT:
            case INPUT:
            case RETURN:
                output << "14 ";
                Statement(output, symbolTable);
                Body(output, symbolTable);
                break;

                // Como BODY -> lambda, Follow (BODY)
            case CURLY_BRACKET_CLOSE:
                output << "15 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Statement(std::ostream& output, SymbolTable& symbolTable) {
        // STATEMENT -> if ( EXP ) ATOMSTATEMENT | for ( FORACT ; EXP ; FORACT ) { BODY } |
        //              var VARTYPE id ; | ATOMSTATEMENT
        switch (m_nextToken.type) {
            // First (if ( EXP ) ATOMSTATEMENT)
            case IF:
                output << "16 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_OPEN)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                AtomStatement(output, symbolTable);
                break;

                // First (for ( FORACT ; EXP ; FORACT ) { BODY })
            case FOR:
                output << "17 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_OPEN)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                ForAct(output, symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                ForAct(output, symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::CURLY_BRACKET_OPEN)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                Body(output, symbolTable);

                if (m_nextToken.type != TokenType::CURLY_BRACKET_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (var VARTYPE id ;)
            case VAR:
                output << "18 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                VarType(output, symbolTable);

                if (m_nextToken.type != IDENTIFIER)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (ATOMSTATEMENT)
            case IDENTIFIER:
            case OUTPUT:
            case INPUT:
            case RETURN:
                output << "19 ";
                AtomStatement(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void AtomStatement(std::ostream& output, SymbolTable& symbolTable) {
        // ATOMSTATEMENT -> id IDACT ; | output EXP ; | input id ; | return RETURNEXP ;
        switch (m_nextToken.type) {
            // First (id IDACT ;)
            case IDENTIFIER:
                output << "20 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                IdAct(output, symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (output EXP ;)
            case OUTPUT:
                output << "21 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (input id ;)
            case INPUT:
                output << "22 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::IDENTIFIER)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (return RETURNEXP ;)
            case RETURN:
                output << "23 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                ReturnExp(output, symbolTable);

                if (m_nextToken.type != TokenType::SEMICOLON)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void IdAct(std::ostream& output, SymbolTable& symbolTable) {
        // IDACT -> ASS EXP | ( CALLPARAM )
        switch (m_nextToken.type) {
            // First (ASS EXP)
            case ASSIGN:
            case CUMULATIVE_ASSIGN:
                output << "24 ";
                Ass(output, symbolTable);
                Exp(output, symbolTable);
                break;

                // First (( CALLPARAM ))
            case PARENTHESIS_OPEN:
                output << "25 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                CallParam(output, symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void ForAct(std::ostream& output, SymbolTable& symbolTable) {
        // FORACT -> id ASS EXP | lambda
        switch (m_nextToken.type) {
            // First (id ASS EXP)
            case IDENTIFIER:
                output << "26 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                Ass(output, symbolTable);
                Exp(output, symbolTable);

                break;

                // Como FORACT -> lambda, Follow (FORACT)
            case PARENTHESIS_CLOSE:
            case SEMICOLON:
                output << "27 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Ass(std::ostream& output, SymbolTable& symbolTable) {
        // ASS -> = | +=
        switch (m_nextToken.type) {
            case ASSIGN:
                output << "28 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            case CUMULATIVE_ASSIGN:
                output << "29 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void CallParam(std::ostream& output, SymbolTable& symbolTable) {
        // CALLPARAM -> EXP NEXTPARAM | lambda
        switch (m_nextToken.type) {
            // First (EXP NEXTPARAM)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << "30 ";
                Exp(output, symbolTable);
                NextParam(output, symbolTable);
                break;

                // Como CALLPARAM -> lambda, Follow (CALLPARAM)
            case PARENTHESIS_CLOSE:
                output << "31 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void NextParam(std::ostream& output, SymbolTable& symbolTable) {
        // NEXTPARAM -> , EXP NEXTPARAM | lambda
        switch (m_nextToken.type) {
            // First (EXP NEXTPARAM)
            case COMMA:
                output << "32 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);
                NextParam(output, symbolTable);

                break;

                // Como NEXTPARAM -> lambda, Follow (NEXTPARAM)
            case PARENTHESIS_CLOSE:
                output << "33 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void ReturnExp(std::ostream& output, SymbolTable& symbolTable) {
        // RETURNEXP -> EXP | lambda
        switch (m_nextToken.type) {
            // First (EXP)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case TRUE:
            case FALSE:
            case IDENTIFIER:
                output << "34 ";
                Exp(output, symbolTable);
                break;

                // Como RETURNEXP -> lambda, Follow (RETURNEXP)
            case SEMICOLON:
                output << "35 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Exp(std::ostream& output, SymbolTable& symbolTable) {
        // EXP -> A EXP1
        switch (m_nextToken.type) {
            // First (A EXP1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << "36 ";
                A(output, symbolTable);
                Exp1(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void Exp1(std::ostream& output, SymbolTable& symbolTable) {
        // EXP1 -> LOGOP A EXP1 | lambda
        switch (m_nextToken.type) {
            // First (LOGOP A EXP1)
            case AND:
            case OR:
                output << "37 ";
                LogOp(output, symbolTable);
                A(output, symbolTable);
                Exp1(output, symbolTable);
                break;

                // Como EXP1 -> lambda, Follow (EXP1)
            case PARENTHESIS_CLOSE:
            case COMMA:
            case SEMICOLON:
                output << "38 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void LogOp(std::ostream& output, SymbolTable& symbolTable) {
        // LOGOP -> && | ||
        switch (m_nextToken.type) {
            case AND:
                output << "39 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            case OR:
                output << "40 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void A(std::ostream& output, SymbolTable& symbolTable) {
        // A -> B A1
        switch (m_nextToken.type) {
            // First (B A1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << "41 ";
                B(output, symbolTable);
                A1(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void A1(std::ostream& output, SymbolTable& symbolTable) {
        // A1 -> COMPOP B A1 | lambda
        switch (m_nextToken.type) {
            // First (COMPOP B A1)
            case GREATER:
            case LESS:
                output << "42 ";
                CompOp(output, symbolTable);
                B(output, symbolTable);
                A1(output, symbolTable);
                break;

                // Como A1 -> lambda, Follow (A1)
            case AND:
            case OR:
            case PARENTHESIS_CLOSE:
            case COMMA:
            case SEMICOLON:
                output << "43 ";
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void CompOp(std::ostream& output, SymbolTable& symbolTable) {
        // COMPOP -> > | <
        switch (m_nextToken.type) {
            // First (>)
            case GREATER:
                output << "44 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (<)
            case LESS:
                output << "45 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void B(std::ostream& output, SymbolTable& symbolTable) {
        // B -> EXPATOM B1
        switch (m_nextToken.type) {
            // First (EXPATOM B1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << "46 ";
                ExpAtom(output, symbolTable);
                B1(output, symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void B1(std::ostream& output, SymbolTable& symbolTable) {
        // B1 -> ARITHMETICOP EXPATOM B1 | lambda
        switch (m_nextToken.type) {
            // First (ARITHMETICOP EXPATOM B1)
            case SUM:
            case SUB:
                output << "47 ";
                ArithmeticOp(output, symbolTable);
                ExpAtom(output, symbolTable);
                B1(output, symbolTable);
                break;

            // Como B1 -> lambda, Follow (B1)
            case AND:
            case OR:
            case PARENTHESIS_CLOSE:
            case COMMA:
            case SEMICOLON:
            case LESS:
            case GREATER:
                output << "48 ";
                break;


            default:
                throw SyntaxException("");
        }
    }

    //
    void ArithmeticOp(std::ostream& output, SymbolTable& symbolTable) {
        // ARITHMETICOP -> + | -
        switch (m_nextToken.type) {
            // First (+)
            case SUM:
                output << "49 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (-)
            case SUB:
                output << "50 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void ExpAtom(std::ostream& output, SymbolTable& symbolTable) {
        // EXPATOM -> id C | ( EXP ) | cint | cstr | true | false
        switch (m_nextToken.type) {
            // First (id C)
            case IDENTIFIER:
                output << "51 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                C(output, symbolTable);
                break;

                // First (( EXP ))
            case PARENTHESIS_OPEN:
                output << "52 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // First (cint)
            case CINT:
                output << "53 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (cstr)
            case CSTR:
                output << "54 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (true)
            case TRUE:
                output << "55 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

                // First (false)
            case FALSE:
                output << "56 ";
                m_nextToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                throw SyntaxException("");
        }
    }

    //
    void C(std::ostream& output, SymbolTable& symbolTable) {
        // C -> ( CALLPARAM ) | lambda
        switch (m_nextToken.type) {
            // First (( CALLPARAM ))
            case PARENTHESIS_OPEN:
                output << "57 ";
                m_nextToken = m_lexer.GetToken(symbolTable);

                CallParam(output, symbolTable);

                if (m_nextToken.type != TokenType::PARENTHESIS_CLOSE)
                    throw SyntaxException("");
                m_nextToken = m_lexer.GetToken(symbolTable);

                break;

                // Como C -> lambda, Follow (C)
            case AND:
            case OR:
            case PARENTHESIS_CLOSE:
            case SUM:
            case SUB:
            case COMMA:
            case SEMICOLON:
            case GREATER:
            case LESS:
                output << "58 ";
                break;

            default:
                throw SyntaxException("");
        }
    }


public:
    explicit Parser(std::istream& input) : m_lexer(input) {
        SymbolTable todo;
    }

    std::string Parse(SymbolTable& symbolTable) {
        std::ostringstream ss;

        try {
            m_nextToken = m_lexer.GetToken(symbolTable);
            Axiom(ss, symbolTable);
        } catch (const SyntaxException& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }

        return ss.str();
    }
};

#pragma clang diagnostic pop