#include "Parser.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

namespace {
    constexpr char aType[] = "type";
    constexpr char aRetType[] = "ret_type";

    constexpr char tVoid[] = "'void'";
    constexpr char tFun[] = "'fun'";
    constexpr char tInt[] = "'int'";
    constexpr char tLog[] = "'log'";
    constexpr char tStr[] = "'str'";

    constexpr char tOk[] = "type_ok";
    constexpr char tError[] = "type_error";
}

// HECHO
RuleAttributes Parser::Axiom(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // P -> FUNCTION P | STATEMENT P | eof
    switch (m_lastToken.type) {
    // First (FUNCTION P)
    case FUNCTION: {
        output << " 1";
        (void) Function(output, symbolTable, globals);
        (void) Axiom(output, symbolTable, globals);
        break;
    }

    // First (STATEMENT P)
    case IF:
    case FOR:
    case VAR:
    case IDENTIFIER:
    case OUTPUT:
    case INPUT:
    case RETURN: {
        output << " 2";
        (void) Statement(output, symbolTable, globals);
        (void) Axiom(output, symbolTable, globals);
        break;
    }

    // First (eof)
    case END: {
        output << " 3";
        break;
    }

    default:
        ThrowError(
            "Instancia incorrecta: "
            "Se esperaba una instancia o una declaración de función."
        );
    }

    return {};
}

RuleAttributes Parser::Function(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    RuleAttributes function = {};

    // FUNCTION -> function FUNTYPE id ( FUNATTRIBUTES ) { BODY }
    output << " 4";
    assert(m_lastToken.type == FUNCTION);

    if (globals.useSemantic) {
        if (globals.localTable.has_value()) {
            throw std::runtime_error("TODO");
        }
    }

    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    const auto funType = FunType(output, symbolTable, globals);
    VerifyTokenType(
        IDENTIFIER,
        "Declaración de función incorrecta: "
        "Se esperaba el nombre de la función."
    );

    if (globals.useSemantic) {
        globals.localTable = SymbolTable(globals.tableCounter);
        globals.tableCounter += 1;
        globals.localOffset = 0;
    }

    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    VerifyTokenType(
        PARENTHESIS_OPEN,
        "Declaración de función incorrecta: "
        "Falta un paréntesis de apertura para declarar los parámetros de la función."
    );
    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    const auto funAttributes = FunAttributes(output, symbolTable, globals);

    VerifyTokenType(
        PARENTHESIS_CLOSE,
        "Declaración de función incorrecta: "
        "Falta un paréntesis de cierre tras declarar los parámetros de la función."
    );
    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    if (globals.useSemantic) {
        // TODO: InsertarTipoTS
        // TODO: InsertarEtiquetaTS
    }

    VerifyTokenType(
        CURLY_BRACKET_OPEN,
        "Declaración de función incorrecta: "
        "Es necesario abrir el cuerpo de la función con una llave «{»."
    );
    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    const auto body = Body(output, symbolTable, globals);

    VerifyTokenType(
        CURLY_BRACKET_CLOSE,
        "Declaración de función incorrecta: "
        "Es necesario cerrar el cuerpo de la función con una llave «}»."
    );
    m_lastToken = m_lexer.GetToken(symbolTable, globals);

    if (globals.useSemantic) {
        if (body.at(aType) != tOk) {
            throw std::runtime_error("TODO");
        }
        if (body.at(aRetType) != funType.at(aType)) {
            throw std::runtime_error("TODO");
        }
        globals.localTable->WriteTable(output);
        globals.localTable = std::nullopt;
    }

    return function;
}

// HECHO
RuleAttributes Parser::FunType(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    RuleAttributes funType = {};

    // FUNTYPE -> RuleAttributes | VARTYPE
    switch (m_lastToken.type) {
    // First (void)
    case VOID: {
        output << " 5";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        if (globals.useSemantic) {
            funType[aType] = tVoid;
        }

        break;
    }

    // First (VARTYPE)
    case INT:
    case BOOL:
    case STRING: {
        output << " 6";
        const auto varType = VarType(output, symbolTable, globals);

        if (globals.useSemantic) {
            funType[aType] = varType.at(aType);
        }

        break;
    }

    default:
        ThrowError(
            "Declaración de función incorrecta: "
            "Se esperaba el tipo de retorno de la función (int, boolean, string) o «void»."
        );
    }

    return funType;
}

// HECHO
RuleAttributes Parser::VarType(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    RuleAttributes varType = {};

    // VARTYPE -> int | boolean | string
    switch (m_lastToken.type) {
    // First (int)
    case INT: {
        output << " 7";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        if (globals.useSemantic) {
            varType[aType] = tInt;
        }

        break;
    }

    // First (bool)
    case BOOL: {
        output << " 8";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        if (globals.useSemantic) {
            varType[aType] = tLog;
        }

        break;
    }

    // First (string)
    case STRING: {
        output << " 9";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        if (globals.useSemantic) {
            varType[aType] = tStr;
        }

        break;
    }

    case VOID:
        ThrowError("Una variable no puede ser de tipo «void».");

    default:
        ThrowError("Tipo de variable desconocido.");
    }

    return varType;
}

// TODO
RuleAttributes Parser::FunAttributes(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // FUNATTRIBUTES -> RuleAttributes | VARTYPE id NEXTATTRIBUTE
    switch (m_lastToken.type) {
    // First (void)
    case VOID:
        output << " 10";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (VARTYPE id NEXTATTRIBUTE)
    case INT:
    case BOOL:
    case STRING:
        output << " 11";
        VarType(output, symbolTable, globals);

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de función incorrecta: "
            "Se espera el nombre del atributo tras declarar su tipo."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        NextAttribute(output, symbolTable, globals);
        break;

    case PARENTHESIS_CLOSE:
        ThrowError(
            "Declaración de función incorrecta: "
            "Es necesario definir algún atributo para la función, o «void» si no toma argumentos."
        );

    default:
        ThrowError(
            std::format(
                "Declaración de función incorrecta: "
                "Elemento de tipo «{}» inesperado en la declaración de atributos de la función.",
                ToString(m_lastToken.type)
            )
        );
    }

    return {};
}

// TODO
RuleAttributes Parser::NextAttribute(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // NEXTATTRIBUTE -> , VARTYPE id NEXTATTRIBUTE | lambda
    switch (m_lastToken.type) {
    // First (, VARTYPE id NEXTATTRIBUTE)
    case COMMA:
        output << " 12";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VarType(output, symbolTable, globals);

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de función incorrecta: "
            "Se espera el nombre del atributo tras declarar su tipo."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        NextAttribute(output, symbolTable, globals);
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

    return {};
}

// HECHO
RuleAttributes Parser::Body(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    RuleAttributes body;

    // BODY -> STATEMENT BODY | lambda
    switch (m_lastToken.type) {
    // First (STATEMENT BODY)
    case IF:
    case FOR:
    case VAR:
    case IDENTIFIER:
    case OUTPUT:
    case INPUT:
    case RETURN: {
        output << " 14";
        const auto statement = Statement(output, symbolTable, globals);
        const auto body_1 = Body(output, symbolTable, globals);

        if (globals.useSemantic) {
            if (statement.at(aType) == tOk) {
                body[aType] = body_1.at(aType);
            } else {
                body[aType] = tError;
            }

            if (statement.at(aRetType) == body_1.at(aRetType) || statement.at(aRetType) == tVoid) {
                body[aRetType] = body_1.at(aRetType);
            } else if (body_1.at(aRetType) == tVoid) {
                body[aRetType] = statement.at(aRetType);
            } else {
                body[aRetType] = tError;
            }
        }

        break;
    }

    // Como BODY -> lambda, Follow (BODY)
    case CURLY_BRACKET_CLOSE: {
        output << " 15";

        if (globals.useSemantic) {
            body[aType] = "type_ok";
            body["ret_type"] = "void";
        }

        break;
    }

    default:
        ThrowError(
            std::format(
                "Sentencia incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return body;
}


RuleAttributes Parser::Statement(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    RuleAttributes statement;

    // STATEMENT -> if ( EXP1 ) ATOMSTATEMENT | for ( FORACT ; EXP1 ; FORACT ) { BODY } |
    //              var VARTYPE id ; | ATOMSTATEMENT
    switch (m_lastToken.type) {
    // First (if ( EXP1 ) ATOMSTATEMENT)
    case IF: {
        output << " 16";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_OPEN,
            "Sentencia if incorrecta: "
            "Se esperaba «(» tras «if»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        const auto exp1 = Exp1(output, symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Sentencia if incorrecta: "
            "Se esperaba «)» tras la condición del «if»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        const auto atomStatement = AtomStatement(output, symbolTable, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) != tLog) {
                statement[aType] = tError;
            } else {
                statement[aType] = atomStatement.at(aType);
            }

            statement[aRetType] = atomStatement.at(aRetType);
        }

        break;
    }

    // First (for ( FORACT ; EXP1 ; FORACT ) { BODY })
    case FOR: {
        output << " 17";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_OPEN,
            "Sentencia for incorrecta: "
            "Se esperaba «(» tras «for»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        (void)ForAct(output, symbolTable, globals);

        VerifyTokenType(
            SEMICOLON,
            "Sentencia for incorrecta: "
            "Las instancias del bucle for deben ir separadas por «;»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp1(output, symbolTable, globals);

        VerifyTokenType(
            SEMICOLON,
            "Sentencia for incorrecta: "
            "Las instancias del bucle for deben ir separadas por «;»."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        ForAct(output, symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Sentencia for incorrecta: "
            "Se esperaba «)» tras declarar las instancias del bucle."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(
            CURLY_BRACKET_OPEN,
            "Sentencia for incorrecta: "
            "Se esperaba «{» para definir el cuerpo del bucle."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Body(output, symbolTable, globals);

        VerifyTokenType(
            CURLY_BRACKET_CLOSE,
            "Sentencia for incorrecta: "
            "Se esperaba «}» para cerrar el cuerpo del bucle."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;
    }

    // First (var VARTYPE id ;)
    case VAR: {
        output << " 18";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VarType(output, symbolTable, globals);

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de variable incorrecta: "
            "Se esperaba un identificador tras el tipo de variable."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(
            SEMICOLON,
            "Declaración de variable incorrecta: "
            "Se esperaba «;» tras declarar la variable."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;
    }

    // First (ATOMSTATEMENT)
    case IDENTIFIER:
    case OUTPUT:
    case INPUT:
    case RETURN: {
        output << " 19";
        AtomStatement(output, symbolTable, globals);
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

    return statement;
}

RuleAttributes Parser::AtomStatement(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // ATOMSTATEMENT -> id IDACT ; | output EXP1 ; | input id ; | return RETURNEXP ;
    switch (m_lastToken.type) {
    // First (id IDACT ;)
    case IDENTIFIER:
        output << " 20";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        IdAct(output, symbolTable, globals);

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    // First (output EXP1 ;)
    case OUTPUT:
        output << " 21";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp1(output, symbolTable, globals);

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    // First (input id ;)
    case INPUT:
        output << " 22";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(IDENTIFIER, "Se esperaba un identificador como parámetro de entrada.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    // First (return RETURNEXP ;)
    case RETURN:
        output << " 23";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        ReturnExp(output, symbolTable, globals);

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

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

    return {};
}

RuleAttributes Parser::IdAct(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // IDACT -> ASS EXP1 | ( CALLPARAM )
    switch (m_lastToken.type) {
    // First (ASS EXP1)
    case ASSIGN:
    case CUMULATIVE_ASSIGN:
        output << " 24";
        Ass(output, symbolTable, globals);
        Exp1(output, symbolTable, globals);
        break;

    // First (( CALLPARAM ))
    case PARENTHESIS_OPEN:
        output << " 25";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        CallParam(output, symbolTable, globals);

        VerifyTokenType(PARENTHESIS_CLOSE, "Falta un paréntesis de cierre en la llamada a la función.");
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    default:
        ThrowError(
            "Sentencia incorrecta: "
            "Se debe realizar una asignación o llamada sobre el identificador."
        );
    }

    return {};
}

RuleAttributes Parser::ForAct(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // FORACT -> id ASS EXP1 | lambda
    switch (m_lastToken.type) {
    // First (id ASS EXP1)
    case IDENTIFIER:
        output << " 26";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Ass(output, symbolTable, globals);
        Exp1(output, symbolTable, globals);

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

    return {};
}

RuleAttributes Parser::Ass(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // ASS -> = | +=
    switch (m_lastToken.type) {
    case ASSIGN:
        output << " 28";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    case CUMULATIVE_ASSIGN:
        output << " 29";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    default:
        ThrowError(
            "Asignación incorrecta: "
            "Se esperaba «=» o «+=»."
        );
    }

    return {};
}

RuleAttributes Parser::CallParam(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // CALLPARAM -> EXP1 NEXTPARAM | lambda
    switch (m_lastToken.type) {
    // First (EXP1 NEXTPARAM)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER:
        output << " 30";
        Exp1(output, symbolTable, globals);
        NextParam(output, symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::NextParam(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // NEXTPARAM -> , EXP1 NEXTPARAM | lambda
    switch (m_lastToken.type) {
    // First (EXP1 NEXTPARAM)
    case COMMA:
        output << " 32";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp1(output, symbolTable, globals);
        NextParam(output, symbolTable, globals);

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

    return {};
}

RuleAttributes Parser::ReturnExp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // RETURNEXP -> EXP1 | lambda
    switch (m_lastToken.type) {
    // First (EXP1)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case TRUE:
    case FALSE:
    case IDENTIFIER:
        output << " 34";
        Exp1(output, symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::Exp1(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXP1 -> EXP2 EXPOR
    switch (m_lastToken.type) {
    // First ( EXP2 EXPOR )
    case PARENTHESIS_OPEN:
    case CSTR:
    case CINT:
    case FALSE:
    case TRUE:
    case IDENTIFIER:
        output << " 36";
        Exp2(output, symbolTable, globals);
        ExpOr(output, symbolTable, globals);
        break;

    default:
        ThrowError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return {};
}

RuleAttributes Parser::ExpOr(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXPOR -> || EXP2 EXPOR | lambda
    switch (m_lastToken.type) {
    // First ( || EXP2 EXPOR )
    case OR:
        output << " 37";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp2(output, symbolTable, globals);
        ExpOr(output, symbolTable, globals);

        break;

    // Como EXPOR -> lambda, Follow (EXPOR)
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
        output << " 38";
        break;

    default:
        ThrowError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return {};
}

RuleAttributes Parser::Exp2(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXP2 -> EXP3 EXPAND
    switch (m_lastToken.type) {
    // First (EXP3 EXPAND)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER:
        output << " 39";
        Exp3(output, symbolTable, globals);
        ExpAnd(output, symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::ExpAnd(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXPAND -> && EXP3 EXPAND | lambda
    switch (m_lastToken.type) {
    // First ( && EXP3 EXPAND )
    case AND:
        output << " 40";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp3(output, symbolTable, globals);
        ExpAnd(output, symbolTable, globals);
        break;

    // Como EXPAND -> lambda, Follow (EXPAND)
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
    case OR:
        output << " 41";
        break;

    default:
        ThrowError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return {};
}

RuleAttributes Parser::Exp3(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXP3 -> EXP4 COMP
    switch (m_lastToken.type) {
    // First (EXP4 COMP)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER:
        output << " 42";
        Exp4(output, symbolTable, globals);
        Comp(output, symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::Comp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // COMP -> COMPOP EXP4 COMP | lambda
    switch (m_lastToken.type) {
    // First (COMPOP EXP4 COMP)
    case GREATER:
    case LESS:
        output << " 43";
        CompOp(output, symbolTable, globals);
        Exp4(output, symbolTable, globals);
        Comp(output, symbolTable, globals);
        break;

    // Como COMP -> lambda, Follow (COMP)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
        output << " 44";
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

    return {};
}

RuleAttributes Parser::CompOp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // COMPOP -> > | <
    switch (m_lastToken.type) {
    // First (>)
    case GREATER:
        output << " 45";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (<)
    case LESS:
        output << " 46";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::Exp4(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXP4 -> EXPATOM ARITH
    switch (m_lastToken.type) {
    // First (EXPATOM ARITH)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER:
        output << " 47";
        ExpAtom(output, symbolTable, globals);
        Arith(output, symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::Arith(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // ARITH -> ARITHOP EXPATOM ARITH | lambda
    switch (m_lastToken.type) {
    // First (ARITHOP EXPATOM ARITH)
    case SUM:
    case SUB:
        output << " 48";
        ArithOp(output, symbolTable, globals);
        ExpAtom(output, symbolTable, globals);
        Arith(output, symbolTable, globals);
        break;

    // Como ARITH -> lambda, Follow (ARITH)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
    case LESS:
    case GREATER:
        output << " 49";
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

    return {};
}

RuleAttributes Parser::ArithOp(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // ARITHOP -> + | -
    switch (m_lastToken.type) {
    // First (+)
    case SUM:
        output << " 50";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (-)
    case SUB:
        output << " 51";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::ExpAtom(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // EXPATOM -> id IDVAL | ( EXP1 ) | cint | cstr | true | false
    switch (m_lastToken.type) {
    // First (id IDVAL)
    case IDENTIFIER:
        output << " 52";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        IdVal(output, symbolTable, globals);
        break;

    // First (( EXP1 ))
    case PARENTHESIS_OPEN:
        output << " 53";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        Exp1(output, symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Expresión incorrecta: "
            "Falta el paréntesis de cierre."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    // First (cint)
    case CINT:
        output << " 54";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (cstr)
    case CSTR:
        output << " 55";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (true)
    case TRUE:
        output << " 56";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
        break;

    // First (false)
    case FALSE:
        output << " 57";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);
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

    return {};
}

RuleAttributes Parser::IdVal(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    // IDVAL -> ( CALLPARAM ) | lambda
    switch (m_lastToken.type) {
    // First (( CALLPARAM ))
    case PARENTHESIS_OPEN:
        output << " 58";
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        CallParam(output, symbolTable, globals);

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Expresión incorrecta: "
            "Falta el paréntesis de cierre."
        );
        m_lastToken = m_lexer.GetToken(symbolTable, globals);

        break;

    // Como IDVAL -> lambda, Follow (IDVAL)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case SUM:
    case SUB:
    case COMMA:
    case SEMICOLON:
    case GREATER:
    case LESS:
        output << " 59";
        break;

    default:
        ThrowError(
            std::format(
                "Expresión incorrecta: "
                "Se esperaba alguna acción sobre el identificador.",
                ToString(m_lastToken.type)
            )
        );
    }

    return {};
}

void Parser::Parse(std::ostream& output, SymbolTables& symbolTable, SemanticState& globals) {
    globals.globalTable = SymbolTable(0);
    globals.tableCounter = 1;
    globals.globalOffset = 0;
    globals.implicitDeclaration = false;

    m_lastToken = m_lexer.GetToken(symbolTable, globals);
    (void) Axiom(output, symbolTable, globals);

    globals.globalTable->WriteTable(output);
    globals.globalTable = std::nullopt;
}

#pragma clang diagnostic pop
