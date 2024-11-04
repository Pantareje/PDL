#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "Lexer.h"
#include "SyntaxException.h"

#include <sstream>
#include <iostream>

class Parser {
    using
    enum TokenType;

    Lexer m_lexer;
    Token m_lastToken;

    [[noreturn]]
    inline void ThrowError(const char* errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }

    [[noreturn]]
    inline void ThrowError(const std::string& errorMessage) const {
        throw SyntaxException(m_lastToken.line, m_lastToken.column, errorMessage);
    }

    inline void VerifyType(TokenType expectedType, const char* errorMessage) const {
        if (m_lastToken.type != expectedType)
            ThrowError(errorMessage);
    }


    void Axiom(std::ostream& output, SymbolTable& symbolTable) {
        // P -> FUNCTION P | STATEMENT P | lambda
        switch (m_lastToken.type) {
            // First (FUNCTION P)
            case FUNCTION:
                output << " 1";
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
                output << " 2";
                Statement(output, symbolTable);
                Axiom(output, symbolTable);
                break;

            // Como P -> lambda, Follow (P)
            case END:
                output << " 3";
                break;

            default:
                ThrowError(
                    "Instancia incorrecta: "
                    "Se esperaba una instancia o una declaración de función."
                );
        }
    }

    void Function(std::ostream& output, SymbolTable& symbolTable) {
        // FUNCTION -> function FUNTYPE id ( FUNATTRIBUTES ) { BODY }

        output << " 4";

        assert(m_lastToken.type == FUNCTION);
        m_lastToken = m_lexer.GetToken(symbolTable);

        FunType(output, symbolTable);

        VerifyType(
            IDENTIFIER,
            "Declaración de función incorrecta: "
            "Se esperaba el nombre de la función."
        );
        m_lastToken = m_lexer.GetToken(symbolTable);

        VerifyType(
            PARENTHESIS_OPEN,
            "Declaración de función incorrecta: "
            "Falta un paréntesis de apertura para declarar los parámetros de la función."
        );
        m_lastToken = m_lexer.GetToken(symbolTable);

        FunAttributes(output, symbolTable);

        VerifyType(
            PARENTHESIS_CLOSE,
            "Declaración de función incorrecta: "
            "Falta un paréntesis de cierre tras declarar los parámetros de la función."
        );
        m_lastToken = m_lexer.GetToken(symbolTable);

        VerifyType(
            CURLY_BRACKET_OPEN,
            "Declaración de función incorrecta: "
            "Es necesario abrir el cuerpo de la función con una llave «{»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable);

        Body(output, symbolTable);

        VerifyType(
            CURLY_BRACKET_CLOSE,
            "Declaración de función incorrecta: "
            "Es necesario cerrar el cuerpo de la función con una llave «}»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable);
    }

    void FunType(std::ostream& output, SymbolTable& symbolTable) {
        // FUNTYPE -> void | VARTYPE
        switch (m_lastToken.type) {
            // First (void)
            case VOID:
                output << " 5";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (VARTYPE)
            case INT:
            case BOOL:
            case STRING:
                output << " 6";
                VarType(output, symbolTable);
                break;

            default:
                ThrowError(
                    "Declaración de función incorrecta: "
                    "se esperaba el tipo de retorno de la función (int, boolean, string) o void."
                );
        }
    }

    void VarType(std::ostream& output, SymbolTable& symbolTable) {
        // VARTYPE -> int | boolean | string
        switch (m_lastToken.type) {
            // First (int)
            case INT:
                output << " 7";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (bool)
            case BOOL:
                output << " 8";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (string)
            case STRING:
                output << " 9";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Tipo de variable «{}» desconocido.",
                        TokenAttributeToString(m_lastToken)
                    )
                );
        }
    }

    void FunAttributes(std::ostream& output, SymbolTable& symbolTable) {
        // FUNATTRIBUTES -> void | VARTYPE id NEXTATTRIBUTE
        switch (m_lastToken.type) {
            // First (void)
            case VOID:
                output << " 10";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (VARTYPE id NEXTATTRIBUTE)
            case INT:
            case BOOL:
            case STRING:
                output << " 11";
                VarType(output, symbolTable);

                VerifyType(
                    IDENTIFIER,
                    "Declaración de función incorrecta: "
                    "Se espera el nombre del atributo tras declarar su tipo."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                NextAttribute(output, symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Declaración de función incorrecta: "
                        "Elemento de tipo «{}» inesperado en la declaración de atributos de la función.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void NextAttribute(std::ostream& output, SymbolTable& symbolTable) {
        // NEXTATTRIBUTE -> , VARTYPE id NEXTATTRIBUTE | lambda
        switch (m_lastToken.type) {
            // First (, VARTYPE id NEXTATTRIBUTE)
            case COMMA:
                output << " 12";
                m_lastToken = m_lexer.GetToken(symbolTable);

                VarType(output, symbolTable);

                VerifyType(
                    IDENTIFIER,
                    "Declaración de función incorrecta: "
                    "Se espera el nombre del atributo tras declarar su tipo."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                NextAttribute(output, symbolTable);
                break;

            // Como NEXTATTRIBUTE -> lambda, Follow (NEXTATTRIBUTE)
            case PARENTHESIS_CLOSE:
                output << " 13";
                break;

            default:
                ThrowError(
                    std::format(
                        "Declaración de función incorrecta: "
                        "Elemento de tipo «{}» inesperado en la declaración de atributos de la función.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void Body(std::ostream& output, SymbolTable& symbolTable) {
        // BODY -> STATEMENT BODY | lambda
        switch (m_lastToken.type) {
            // First (STATEMENT BODY)
            case IF:
            case FOR:
            case VAR:
            case IDENTIFIER:
            case OUTPUT:
            case INPUT:
            case RETURN:
                output << " 14";
                Statement(output, symbolTable);
                Body(output, symbolTable);
                break;

            // Como BODY -> lambda, Follow (BODY)
            case CURLY_BRACKET_CLOSE:
                output << " 15";
                break;

            default:
                ThrowError(
                    std::format(
                        "Sentencia incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void Statement(std::ostream& output, SymbolTable& symbolTable) {
        // STATEMENT -> if ( EXP ) ATOMSTATEMENT | for ( FORACT ; EXP ; FORACT ) { BODY } |
        //              var VARTYPE id ; | ATOMSTATEMENT
        switch (m_lastToken.type) {
            // First (if ( EXP ) ATOMSTATEMENT)
            case IF:
                output << " 16";
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(
                    PARENTHESIS_OPEN,
                    "Sentencia if incorrecta: "
                    "Se esperaba un paréntesis de apertura tras «if»."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                VerifyType(
                    PARENTHESIS_CLOSE,
                    "Sentencia if incorrecta: "
                    "Se esperaba «)» tras la condición del «if»."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                AtomStatement(output, symbolTable);
                break;

            // First (for ( FORACT ; EXP ; FORACT ) { BODY })
            case FOR:
                output << " 17";
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(
                    PARENTHESIS_OPEN,
                    "Sentencia for incorrecta: "
                    "Se esperaba «(» tras «for»."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                ForAct(output, symbolTable);

                VerifyType(
                    SEMICOLON,
                    "Sentencia for incorrecta: "
                    "Las instancias del bucle for deben ir separadas por «;»."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                VerifyType(
                    SEMICOLON,
                    "Sentencia for incorrecta: "
                    "Las instancias del bucle for deben ir separadas por «;»."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                ForAct(output, symbolTable);

                VerifyType(
                    PARENTHESIS_CLOSE,
                    "Sentencia for incorrecta: "
                    "Se esperaba «)» tras declarar las instancias del bucle."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(
                    CURLY_BRACKET_OPEN,
                    "Sentencia for incorrecta: "
                    "Se esperaba «{» para definir el cuerpo del bucle."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                Body(output, symbolTable);

                VerifyType(
                    CURLY_BRACKET_CLOSE,
                    "Sentencia for incorrecta: "
                    "Se esperaba «}» para cerrar el cuerpo del bucle."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (var VARTYPE id ;)
            case VAR:
                output << " 18";
                m_lastToken = m_lexer.GetToken(symbolTable);

                VarType(output, symbolTable);

                VerifyType(
                    IDENTIFIER,
                    "Declaración de variable incorrecta: "
                    "Se esperaba un identificador tras el tipo de variable."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(
                    SEMICOLON,
                   "Declaración de variable incorrecta: "
                   "Se esperaba «;» tras declarar la variable."
               );
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (ATOMSTATEMENT)
            case IDENTIFIER:
            case OUTPUT:
            case INPUT:
            case RETURN:
                output << " 19";
                AtomStatement(output, symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Sentencia incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void AtomStatement(std::ostream& output, SymbolTable& symbolTable) {
        // ATOMSTATEMENT -> id IDACT ; | output EXP ; | input id ; | return RETURNEXP ;
        switch (m_lastToken.type) {
            // First (id IDACT ;)
            case IDENTIFIER:
                output << " 20";
                m_lastToken = m_lexer.GetToken(symbolTable);

                IdAct(output, symbolTable);

                VerifyType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (output EXP ;)
            case OUTPUT:
                output << " 21";
                m_lastToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                VerifyType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (input id ;)
            case INPUT:
                output << " 22";
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(SEMICOLON, "Se esperaba un identificador como parámetro de entrada.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                VerifyType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (return RETURNEXP ;)
            case RETURN:
                output << " 23";
                m_lastToken = m_lexer.GetToken(symbolTable);

                ReturnExp(output, symbolTable);

                VerifyType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            default:
                ThrowError(
                    std::format(
                        "Sentencia incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void IdAct(std::ostream& output, SymbolTable& symbolTable) {
        // IDACT -> ASS EXP | ( CALLPARAM )
        switch (m_lastToken.type) {
            // First (ASS EXP)
            case ASSIGN:
            case CUMULATIVE_ASSIGN:
                output << " 24";
                Ass(output, symbolTable);
                Exp(output, symbolTable);
                break;

            // First (( CALLPARAM ))
            case PARENTHESIS_OPEN:
                output << " 25";
                m_lastToken = m_lexer.GetToken(symbolTable);

                CallParam(output, symbolTable);

                VerifyType(PARENTHESIS_CLOSE, "Falta un paréntesis de cierre en la llamada a la función.");
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            default:
                ThrowError(
                    "Sentencia incorrecta: "
                    "Se debe realizar una asignación o llamada sobre el identificador."
                );
        }
    }

    void ForAct(std::ostream& output, SymbolTable& symbolTable) {
        // FORACT -> id ASS EXP | lambda
        switch (m_lastToken.type) {
            // First (id ASS EXP)
            case IDENTIFIER:
                output << " 26";
                m_lastToken = m_lexer.GetToken(symbolTable);

                Ass(output, symbolTable);
                Exp(output, symbolTable);

                break;

            // Como FORACT -> lambda, Follow (FORACT)
            case PARENTHESIS_CLOSE:
            case SEMICOLON:
                output << " 27";
                break;

            default:
                ThrowError(
                    std::format(
                        "Acción incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void Ass(std::ostream& output, SymbolTable& symbolTable) {
        // ASS -> = | +=
        switch (m_lastToken.type) {
            case ASSIGN:
                output << " 28";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            case CUMULATIVE_ASSIGN:
                output << " 29";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    "Asignación incorrecta: "
                    "Se esperaba «=» o «+=»."
                );
        }
    }

    void CallParam(std::ostream& output, SymbolTable& symbolTable) {
        // CALLPARAM -> EXP NEXTPARAM | lambda
        switch (m_lastToken.type) {
            // First (EXP NEXTPARAM)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << " 30";
                Exp(output, symbolTable);
                NextParam(output, symbolTable);
                break;

            // Como CALLPARAM -> lambda, Follow (CALLPARAM)
            case PARENTHESIS_CLOSE:
                output << " 31";
                break;

            default:
                ThrowError(
                    std::format(
                        "Lista de parámetros incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void NextParam(std::ostream& output, SymbolTable& symbolTable) {
        // NEXTPARAM -> , EXP NEXTPARAM | lambda
        switch (m_lastToken.type) {
            // First (EXP NEXTPARAM)
            case COMMA:
                output << " 32";
                m_lastToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);
                NextParam(output, symbolTable);

                break;

            // Como NEXTPARAM -> lambda, Follow (NEXTPARAM)
            case PARENTHESIS_CLOSE:
                output << " 33";
                break;

            default:
                ThrowError(
                    std::format(
                        "Lista de parámetros incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void ReturnExp(std::ostream& output, SymbolTable& symbolTable) {
        // RETURNEXP -> EXP | lambda
        switch (m_lastToken.type) {
            // First (EXP)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case TRUE:
            case FALSE:
            case IDENTIFIER:
                output << " 34";
                Exp(output, symbolTable);
                break;

            // Como RETURNEXP -> lambda, Follow (RETURNEXP)
            case SEMICOLON:
                output << " 35";
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión de retorno incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void Exp(std::ostream& output, SymbolTable& symbolTable) {
        // EXP -> A EXP1
        switch (m_lastToken.type) {
            // First (A EXP1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << " 36";
                A(output, symbolTable);
                Exp1(output, symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void Exp1(std::ostream& output, SymbolTable& symbolTable) {
        // EXP1 -> LOGOP A EXP1 | lambda
        switch (m_lastToken.type) {
            // First (LOGOP A EXP1)
            case AND:
            case OR:
                output << " 37";
                LogOp(output, symbolTable);
                A(output, symbolTable);
                Exp1(output, symbolTable);
                break;

            // Como EXP1 -> lambda, Follow (EXP1)
            case PARENTHESIS_CLOSE:
            case COMMA:
            case SEMICOLON:
                output << " 38";
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void LogOp(std::ostream& output, SymbolTable& symbolTable) {
        // LOGOP -> && | ||
        switch (m_lastToken.type) {
            case AND:
                output << " 39";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            case OR:
                output << " 40";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Operación lógica incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void A(std::ostream& output, SymbolTable& symbolTable) {
        // A -> B A1
        switch (m_lastToken.type) {
            // First (B A1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << " 41";
                B(output, symbolTable);
                A1(output, symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void A1(std::ostream& output, SymbolTable& symbolTable) {
        // A1 -> COMPOP B A1 | lambda
        switch (m_lastToken.type) {
            // First (COMPOP B A1)
            case GREATER:
            case LESS:
                output << " 42";
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
                output << " 43";
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void CompOp(std::ostream& output, SymbolTable& symbolTable) {
        // COMPOP -> > | <
        switch (m_lastToken.type) {
            // First (>)
            case GREATER:
                output << " 44";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (<)
            case LESS:
                output << " 45";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Comparación incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void B(std::ostream& output, SymbolTable& symbolTable) {
        // B -> EXPATOM B1
        switch (m_lastToken.type) {
            // First (EXPATOM B1)
            case PARENTHESIS_OPEN:
            case CINT:
            case CSTR:
            case FALSE:
            case TRUE:
            case IDENTIFIER:
                output << " 46";
                ExpAtom(output, symbolTable);
                B1(output, symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void B1(std::ostream& output, SymbolTable& symbolTable) {
        // B1 -> ARITHMETICOP EXPATOM B1 | lambda
        switch (m_lastToken.type) {
            // First (ARITHMETICOP EXPATOM B1)
            case SUM:
            case SUB:
                output << " 47";
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
                output << " 48";
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void ArithmeticOp(std::ostream& output, SymbolTable& symbolTable) {
        // ARITHMETICOP -> + | -
        switch (m_lastToken.type) {
            // First (+)
            case SUM:
                output << " 49";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (-)
            case SUB:
                output << " 50";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Operación aritmética incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void ExpAtom(std::ostream& output, SymbolTable& symbolTable) {
        // EXPATOM -> id C | ( EXP ) | cint | cstr | true | false
        switch (m_lastToken.type) {
            // First (id C)
            case IDENTIFIER:
                output << " 51";
                m_lastToken = m_lexer.GetToken(symbolTable);

                C(output, symbolTable);
                break;

            // First (( EXP ))
            case PARENTHESIS_OPEN:
                output << " 52";
                m_lastToken = m_lexer.GetToken(symbolTable);

                Exp(output, symbolTable);

                VerifyType(
                    PARENTHESIS_CLOSE,
                    "Expresión incorrecta: "
                    "Falta el paréntesis de cierre."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

                break;

            // First (cint)
            case CINT:
                output << " 53";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (cstr)
            case CSTR:
                output << " 54";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (true)
            case TRUE:
                output << " 55";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            // First (false)
            case FALSE:
                output << " 56";
                m_lastToken = m_lexer.GetToken(symbolTable);
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }

    void C(std::ostream& output, SymbolTable& symbolTable) {
        // C -> ( CALLPARAM ) | lambda
        switch (m_lastToken.type) {
            // First (( CALLPARAM ))
            case PARENTHESIS_OPEN:
                output << " 57";
                m_lastToken = m_lexer.GetToken(symbolTable);

                CallParam(output, symbolTable);

                VerifyType(
                    PARENTHESIS_CLOSE,
                    "Expresión incorrecta: "
                    "Falta el paréntesis de cierre."
                );
                m_lastToken = m_lexer.GetToken(symbolTable);

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
                output << " 58";
                break;

            default:
                ThrowError(
                    std::format(
                        "Expresión incorrecta: "
                        "Elemento de tipo «{}» inesperado.",
                        ToString(m_lastToken.type)
                    )
                );
        }
    }


public:
    explicit Parser(std::istream& input) : m_lexer(input) {
        SymbolTable todo;
    }

    std::string Parse(SymbolTable& symbolTable) {
        std::ostringstream ss;

        try {
            m_lastToken = m_lexer.GetToken(symbolTable);
            Axiom(ss, symbolTable);
        } catch (const SyntaxException& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }

        return ss.str();
    }
};

#pragma clang diagnostic pop