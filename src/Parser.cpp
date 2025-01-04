#include "Parser.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

namespace {
    void WriteParse(std::ostream& output, const GlobalState& globals, int transition) {
        if (globals.syntaxPrint == SyntaxPrint::PARSE)
            output << ' ' << transition;
    }

    void WriteCurrentTable(std::ostream& output, const GlobalState& globals) {
        if (globals.syntaxPrint == SyntaxPrint::SYMBOLS) {
            if (globals.localTable.has_value()) {
                globals.localTable->WriteTable(output);
            } else {
                globals.globalTable->WriteTable(output);
            }
        }

        output << std::endl;
    }

    [[nodiscard]] bool IsFunction(const ValueProduct& type) {
        const auto& values = type.GetStrings();
        assert(!values.empty());
        const bool res = values[0] == tFun;
        assert(!res || values.size() >= 2);
        return res;
    }

    [[nodiscard]] ValueProduct GetFunctionReturnType(const ValueProduct& type) {
        assert(IsFunction(type));
        const auto& values = type.GetStrings();
        return ValueProduct { values[1] };
    }

    [[nodiscard]] ValueProduct GetFunctionArgsType(const ValueProduct& type) {
        assert(IsFunction(type));

        const auto& values = type.GetStrings();

        ValueProduct result = {};

        if (values.size() == 2) {
            result.Append(tVoid);
        } else {
            for (size_t i = 2; i < values.size(); i++) {
                result.Append(values[i]);
            }
        }

        return result;
    }
}

using enum TokenType;

// HECHO
RuleAttributes Parser::Axiom(std::ostream& output, GlobalState& globals) {
    RuleAttributes axiom = {};

    // P -> FUNCTION P | STATEMENT P | eof
    switch (m_lastToken.type) {
    // First (FUNCTION P)
    case FUNCTION: {
        WriteParse(output, globals, 1);

        // ------ //
        
        (void) Function(output, globals);

        // ------ //
        
        (void) Axiom(output, globals);

        // ------ //
        
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
        WriteParse(output, globals, 2);

        // ------ //
        
        (void) Statement(output, globals);

        // ------ //
        
        (void) Axiom(output, globals);

        // ------ //
        
        break;
    }

    // First (eof)
    case END: {
        WriteParse(output, globals, 3);

        // ------ //
        
        break;
    }

    default:
        ThrowSyntaxError(
            "Instancia incorrecta: "
            "Se esperaba una instancia o una declaración de función."
        );
    }

    return axiom;
}

// HECHO
RuleAttributes Parser::Function(std::ostream& output, GlobalState& globals) {
    RuleAttributes function = {};

    // FUNCTION -> function FUNTYPE id ( FUNATTRIBUTES ) { BODY }

    assert(m_lastToken.type == FUNCTION);
    WriteParse(output, globals, 4);

    // ------ //

    if (globals.useSemantic) {
        if (globals.localTable.has_value()) {
            LogSemanticError(globals, "TODO: NO SE PERMITEN FUNCIONES ANIDADAS."); // TODO
        }

        globals.implicitDeclaration = false;
    }

    GetNextToken(globals);

    // ------ //

    const auto funType = FunType(output, globals);

    // ------ //

    VerifyTokenType(
        IDENTIFIER,
        "Declaración de función incorrecta: "
        "Se esperaba el nombre de la función."
    );

    const auto id = m_lastToken;

    if (globals.useSemantic) {
        globals.localTable = SymbolTable(globals.tableCounter);
        globals.tableCounter += 1;
        globals.localOffset = 0;
    }

    GetNextToken(globals);

    // ------ //

    VerifyTokenType(
        PARENTHESIS_OPEN,
        "Declaración de función incorrecta: "
        "Falta un paréntesis de apertura para declarar los parámetros de la función."
    );

    GetNextToken(globals);

    // ------ //

    const auto funAttributes = FunAttributes(output, globals);

    if (globals.useSemantic) {
        const auto& pos = std::get<SymbolPos>(id.attribute);

        if (globals.HasType(pos)) {
            LogSemanticError(globals, "TODO: YA EXISTE UN IDENTIDICADOR CON ESE NOMBRE."); // TODO
        } else {
            globals.AddFunctionType(pos, funType.at(aType), funAttributes.at(aType));
        }

        globals.implicitDeclaration = true;
    }

    // ------ //

    VerifyTokenType(
        PARENTHESIS_CLOSE,
        "Declaración de función incorrecta: "
        "Falta un paréntesis de cierre tras declarar los parámetros de la función."
    );

    GetNextToken(globals);

    // ------ //

    VerifyTokenType(
        CURLY_BRACKET_OPEN,
        "Declaración de función incorrecta: "
        "Es necesario abrir el cuerpo de la función con una llave «{»."
    );

    GetNextToken(globals);

    // ------ //

    const auto body = Body(output, globals);

    // ------ //

    VerifyTokenType(
        CURLY_BRACKET_CLOSE,
        "Declaración de función incorrecta: "
        "Es necesario cerrar el cuerpo de la función con una llave «}»."
    );

    if (globals.useSemantic) {
        if (body.at(aType) != tOk) {
            LogSemanticError(globals, "TODO: ERROR EN EL CUERPO DE LA FUNCIÓN"); // TODO
        }
        if (body.at(aRetType) != funType.at(aType)) {
            LogSemanticError(globals, "TODO: TIPO DE RETORNO INCOHERENTE"); // TODO
        }

        WriteCurrentTable(output, globals);
        globals.localTable = std::nullopt;
    }

    GetNextToken(globals);

    // ------ //

    return function;
}

// HECHO
RuleAttributes Parser::FunType(std::ostream& output, GlobalState& globals) {
    RuleAttributes funType = {};

    // FUNTYPE -> RuleAttributes | VARTYPE
    switch (m_lastToken.type) {
    // First (void)
    case VOID: {
        WriteParse(output, globals, 5);

        // ------ //

        if (globals.useSemantic) {
            funType[aType] = tVoid;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (VARTYPE)
    case INT:
    case BOOL:
    case STRING: {
        WriteParse(output, globals, 6);

        // ------ //

        const auto varType = VarType(output, globals);

        if (globals.useSemantic) {
            funType[aType] = varType.at(aType);
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            "Declaración de función incorrecta: "
            "Se esperaba el tipo de retorno de la función (int, boolean, string) o «void»."
        );
    }

    return funType;
}

// HECHO
RuleAttributes Parser::VarType(std::ostream& output, GlobalState& globals) {
    RuleAttributes varType = {};

    // VARTYPE -> int | boolean | string
    switch (m_lastToken.type) {
    // First (int)
    case INT: {
        WriteParse(output, globals, 7);

        // ------ //

        if (globals.useSemantic) {
            varType[aType] = tInt;
            varType[aWidth] = wInt;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (bool)
    case BOOL: {
        WriteParse(output, globals, 8);

        // ------ //

        if (globals.useSemantic) {
            varType[aType] = tLog;
            varType[aWidth] = wLog;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (string)
    case STRING: {
        WriteParse(output, globals, 9);

        // ------ //

        if (globals.useSemantic) {
            varType[aType] = tStr;
            varType[aWidth] = wStr;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    case VOID:
        ThrowSyntaxError("Una variable no puede ser de tipo «void».");

    default:
        ThrowSyntaxError("Tipo de variable desconocido.");
    }

    return varType;
}

// HECHO
RuleAttributes Parser::FunAttributes(std::ostream& output, GlobalState& globals) {
    RuleAttributes funAttributes = {};

    // FUNATTRIBUTES -> RuleAttributes | VARTYPE id NEXTATTRIBUTES
    switch (m_lastToken.type) {
    // First (void)
    case VOID: {
        WriteParse(output, globals, 10);

        // ------ //

        if (globals.useSemantic) {
            funAttributes[aType] = tVoid;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (VARTYPE id NEXTATTRIBUTES)
    case INT:
    case BOOL:
    case STRING: {
        WriteParse(output, globals, 11);

        // ------ //

        const auto varType = VarType(output, globals);

        // ------ //

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de función incorrecta: "
            "Se espera el nombre del atributo tras declarar su tipo."
        );

        const auto id = m_lastToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            if (globals.HasType(pos)) {
                funAttributes[aType] = tError;
                LogSemanticError(globals, "TODO: YA EXISTE UN ATRIBUTO CON ESE NOMBRE."); // TODO
            } else {
                funAttributes[aType] = tOk;
                globals.AddType(pos, varType.at(aType));
                globals.AddOffset(pos, globals.localOffset);
                globals.localOffset += varType.at(aWidth);
            }
        }

        GetNextToken(globals);

        // ------ //

        const auto nextAttributes = NextAttributes(output, globals);

        if (globals.useSemantic) {
            if (funAttributes.at(aType) == tError || nextAttributes.at(aType) == tError) {
                funAttributes[aType] = tError;
            } else if (nextAttributes.at(aType) != tVoid) {
                funAttributes[aType] = varType.at(aType) * nextAttributes.at(aType);
            } else {
                funAttributes[aType] = varType.at(aType);
            }
        }

        // ------ //

        break;
    }

    case PARENTHESIS_CLOSE:
        ThrowSyntaxError(
            "Declaración de función incorrecta: "
            "Es necesario definir algún atributo para la función, o «void» si no toma argumentos."
        );

    default:
        ThrowSyntaxError(
            std::format(
                "Declaración de función incorrecta: "
                "Elemento de tipo «{}» inesperado en la declaración de atributos de la función.",
                ToString(m_lastToken.type)
            )
        );
    }

    return funAttributes;
}

// HECHO
RuleAttributes Parser::NextAttributes(std::ostream& output, GlobalState& globals) {
    RuleAttributes nextAttributes = {};

    // NEXTATTRIBUTES -> , VARTYPE id NEXTATTRIBUTES | lambda
    switch (m_lastToken.type) {
    // First (, VARTYPE id NEXTATTRIBUTES)
    case COMMA: {
        WriteParse(output, globals, 12);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto varType = VarType(output, globals);

        // ------ //

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de función incorrecta: "
            "Se espera el nombre del atributo tras declarar su tipo."
        );

        const auto id = m_lastToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            if (globals.HasType(pos)) {
                nextAttributes[aType] = tError;
                LogSemanticError(globals, "TODO: YA EXISTE UN ATRIBUTO CON ESE NOMBRE."); // TODO
            } else {
                nextAttributes[aType] = tOk;
                globals.AddType(pos, varType.at(aType));
                globals.AddOffset(pos, globals.localOffset);
                globals.localOffset += varType.at(aWidth);
            }
        }

        GetNextToken(globals);

        // ------ //

        const auto nextAttributes_1 = NextAttributes(output, globals);

        if (globals.useSemantic) {
            if (nextAttributes.at(aType) == tError || nextAttributes_1.at(aType) == tError) {
                nextAttributes[aType] = tError;
            } else if (nextAttributes_1.at(aType) != tVoid) {
                nextAttributes[aType] = varType.at(aType) * nextAttributes_1.at(aType);
            } else {
                nextAttributes[aType] = varType.at(aType);
            }
        }

        // ------ //

        break;
    }

    // Como NEXTATTRIBUTES -> lambda, Follow (NEXTATTRIBUTES)
    case PARENTHESIS_CLOSE: {
        WriteParse(output, globals, 13);

        // ------ //

        if (globals.useSemantic) {
            nextAttributes[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Declaración de función incorrecta: "
                "Elemento de tipo «{}» inesperado en la declaración de atributos de la función.",
                ToString(m_lastToken.type)
            )
        );
    }

    return nextAttributes;
}

// HECHO
RuleAttributes Parser::Body(std::ostream& output, GlobalState& globals) {
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
        WriteParse(output, globals, 14);

        // ------ //

        const auto statement = Statement(output, globals);

        // ------ //

        const auto body_1 = Body(output, globals);

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

        // ------ //

        break;
    }

    // Como BODY -> lambda, Follow (BODY)
    case CURLY_BRACKET_CLOSE: {
        WriteParse(output, globals, 15);

        // ------ //

        if (globals.useSemantic) {
            body[aType] = tOk;
            body[aRetType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Sentencia incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return body;
}

// HECHO
RuleAttributes Parser::Statement(std::ostream& output, GlobalState& globals) {
    RuleAttributes statement;

    // STATEMENT -> if ( EXP1 ) ATOMSTATEMENT | for ( FORACT ; EXP1 ; FORACT ) { BODY } |
    //              var VARTYPE id ; | ATOMSTATEMENT
    switch (m_lastToken.type) {
    // First (if ( EXP1 ) ATOMSTATEMENT)
    case IF: {
        WriteParse(output, globals, 16);

        // ------ //

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(
            PARENTHESIS_OPEN,
            "Sentencia if incorrecta: "
            "Se esperaba «(» tras «if»."
        );

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        // ------ //

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Sentencia if incorrecta: "
            "Se esperaba «)» tras la condición del «if»."
        );

        GetNextToken(globals);

        // ------ //

        const auto atomStatement = AtomStatement(output, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) != tLog) {
                statement[aType] = tError;
            } else {
                statement[aType] = atomStatement.at(aType);
            }

            statement[aRetType] = atomStatement.at(aRetType);
        }

        // ------ //

        break;
    }

    // First (for ( FORACT ; EXP1 ; FORACT ) { BODY })
    case FOR: {
        WriteParse(output, globals, 17);

        // ------ //

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(
            PARENTHESIS_OPEN,
            "Sentencia for incorrecta: "
            "Se esperaba «(» tras «for»."
        );

        GetNextToken(globals);

        // ------ //

        const auto forAct_1 = ForAct(output, globals);

        // ------ //

        VerifyTokenType(
            SEMICOLON,
            "Sentencia for incorrecta: "
            "Las instancias del bucle for deben ir separadas por «;»."
        );

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        // ------ //

        VerifyTokenType(
            SEMICOLON,
            "Sentencia for incorrecta: "
            "Las instancias del bucle for deben ir separadas por «;»."
        );

        GetNextToken(globals);

        // ------ //

        const auto forAct_2 = ForAct(output, globals);

        // ------ //

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Sentencia for incorrecta: "
            "Se esperaba «)» tras declarar las instancias del bucle."
        );

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(
            CURLY_BRACKET_OPEN,
            "Sentencia for incorrecta: "
            "Se esperaba «{» para definir el cuerpo del bucle."
        );

        GetNextToken(globals);

        // ------ //

        const auto body = Body(output, globals);

        // ------ //

        VerifyTokenType(
            CURLY_BRACKET_CLOSE,
            "Sentencia for incorrecta: "
            "Se esperaba «}» para cerrar el cuerpo del bucle."
        );

        if (globals.useSemantic) {
            if (forAct_1.at(aType) != tOk) {
                statement[aType] = tError;
                LogSemanticError(globals, "TODO 1"); // TODO
            } else if (exp1.at(aType) != tLog) {
                statement[aType] = tError;
                LogSemanticError(globals, "TODO 2"); // TODO
            } else if (forAct_2.at(aType) != tOk) {
                statement[aType] = tError;
                LogSemanticError(globals, "TODO 3"); // TODO
            } else {
                statement[aType] = body.at(aType);
            }

            statement[aRetType] = body.at(aRetType);
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (var VARTYPE id ;)
    case VAR: {
        // TODO: Arreglar acc sem.
        WriteParse(output, globals, 18);

        // ------ //

        GetNextToken(globals);

        if (globals.useSemantic) {
            globals.implicitDeclaration = false;
        }

        // ------ //

        const auto varType = VarType(output, globals);

        // ------ //

        VerifyTokenType(
            IDENTIFIER,
            "Declaración de variable incorrecta: "
            "Se esperaba un identificador tras el tipo de variable."
        );

        const auto id = m_lastToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            statement[aRetType] = tVoid;

            if (globals.HasType(pos)) {
                statement[aType] = tError;
                LogSemanticError(globals, "TODO: LA VARIABLE YA EXISTE"); // TODO
            } else {
                globals.AddType(pos, varType.at(aType));
                auto& offset = globals.CurrentOffset();
                globals.AddOffset(pos, offset);
                offset += varType.at(aWidth);
                statement[aType] = tOk;
            }

            globals.implicitDeclaration = true;
        }

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(
            SEMICOLON,
            "Declaración de variable incorrecta: "
            "Se esperaba «;» tras declarar la variable."
        );

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (ATOMSTATEMENT)
    case IDENTIFIER:
    case OUTPUT:
    case INPUT:
    case RETURN: {
        WriteParse(output, globals, 19);

        // ------ //

        const auto atomStatement = AtomStatement(output, globals);

        if (globals.useSemantic) {
            statement[aType] = atomStatement.at(aType);
            statement[aRetType] = atomStatement.at(aRetType);
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Sentencia incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return statement;
}

// HECHO
RuleAttributes Parser::AtomStatement(std::ostream& output, GlobalState& globals) {
    RuleAttributes atomStatement;

    // ATOMSTATEMENT -> id IDACT ; | output EXP1 ; | input id ; | return RETURNEXP ;
    switch (m_lastToken.type) {
    // First (id IDACT ;)
    case IDENTIFIER: {
        WriteParse(output, globals, 20);

        // ------ //

        const auto id = m_lastToken;

        GetNextToken(globals);

        // ------ //

        const auto idAct = IdAct(output, globals);

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            atomStatement[aRetType] = tVoid;

            if (idAct.at(aFunCall)) {
                if (!IsFunction(type)) {
                    atomStatement[aType] = tError;
                    LogSemanticError(globals, "TODO: NO SE PUEDE LLAMAR USAR UNA VARIABLE COMO FUNCIÓN"); // TODO
                } else if (idAct.at(aType) == tError) {
                    atomStatement[aType] = tError;
                } else if (idAct.at(aType) == GetFunctionArgsType(type)) {
                    atomStatement[aType] = tOk;
                } else {
                    atomStatement[aType] = tError;
                    LogSemanticError(globals, "TODO: NO COINCIDEN LOS TIPOS EN LA LLAMADA A LA FUNCIÓN"); // TODO
                }
            } else {
                if (IsFunction(type)) {
                    atomStatement[aType] = tError;
                    LogSemanticError(globals, "TODO: NO SE PUEDE USAR UNA FUNCIÓN COMO VARIABLE"); // TODO
                } else if (idAct.at(aType) == tError) {
                    atomStatement[aType] = tError;
                } else if (idAct.at(aType) == type) {
                    atomStatement[aType] = tOk;
                } else {
                    atomStatement[aType] = tError;
                    LogSemanticError(globals, "TODO: LOS TIPOS DE LA ASIGNACIÓN NO COINCIDEN"); // TODO
                }
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (output EXP1 ;)
    case OUTPUT: {
        WriteParse(output, globals, 21);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            atomStatement[aRetType] = tVoid;

            if (exp1.at(aType) == tError) {
                atomStatement[aType] == tError;
            } else if (exp1.at(aType) == tStr || exp1.at(aType) == tInt) {
                atomStatement[aType] = tOk;
            } else {
                atomStatement[aType] = tError;
                LogSemanticError(globals, "TODO: TIPO ILEGAL PARA OUTPUT"); // TODO
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (input id ;)
    case INPUT: {
        WriteParse(output, globals, 22);

        // ------ //

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(IDENTIFIER, "Se esperaba un identificador como parámetro de entrada.");

        const auto id = m_lastToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            atomStatement[aRetType] = tVoid;

            if (type == tStr || type == tInt) {
                atomStatement[aType] = tOk;
            } else {
                atomStatement[aType] = tError;
                LogSemanticError(globals, "TODO: TIPO ILEGAL PARA INPUT"); // TODO
            }
        }

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (return RETURNEXP ;)
    case RETURN: {
        WriteParse(output, globals, 23);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto returnExp = ReturnExp(output, globals);

        if (globals.useSemantic) {
            if (returnExp.at(aType) != tError) {
                atomStatement[aType] = tOk;
            } else {
                atomStatement[aType] = tError;
            }

            atomStatement[aRetType] = returnExp.at(aType);
        }

        // ------ //

        VerifyTokenType(SEMICOLON, "Se esperaba «;» para finalizar la sentencia.");

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Sentencia incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return atomStatement;
}

// HECHO
RuleAttributes Parser::IdAct(std::ostream& output, GlobalState& globals) {
    RuleAttributes idAct = {};

    // IDACT -> ASS EXP1 | ( CALLPARAMS )
    switch (m_lastToken.type) {
    // First (ASS EXP1)
    case ASSIGN:
    case CUMULATIVE_ASSIGN: {
        WriteParse(output, globals, 24);

        // ------ //

        const auto ass = Ass(output, globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            const auto& type = exp1.at(aType);

            idAct[aFunCall] = false;

            if (ass.at(aSum)) {
                if (type == tInt || type == tStr) {
                    idAct[aType] = type;
                } else {
                    idAct[aType] = tError;
                }
            } else {
                idAct[aType] = type;
            }
        }

        // ------ //

        break;
    }

    // First (( CALLPARAMS ))
    case PARENTHESIS_OPEN: {
        WriteParse(output, globals, 25);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto callParams = CallParams(output, globals);

        if (globals.useSemantic) {
            idAct[aFunCall] = true;
            idAct[aType] = callParams.at(aType);
        }

        // ------ //

        VerifyTokenType(PARENTHESIS_CLOSE, "Falta un paréntesis de cierre en la llamada a la función.");

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            "Sentencia incorrecta: "
            "Se debe realizar una asignación o llamada sobre el identificador."
        );
    }

    return idAct;
}

// HECHO
RuleAttributes Parser::ForAct(std::ostream& output, GlobalState& globals) {
    RuleAttributes forAct = {};

    // FORACT -> id ASS EXP1 | lambda
    switch (m_lastToken.type) {
    // First (id ASS EXP1)
    case IDENTIFIER: {
        WriteParse(output, globals, 26);

        // ------ //

        const auto id = m_lastToken;

        GetNextToken(globals);

        // ------ //

        const auto ass = Ass(output, globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            if (type != tInt) {
                forAct[aType] = tError;
                LogSemanticError(globals, "TODO 1"); // TODO
            } else if (exp1.at(aType) == tError) {
                forAct[aType] = tError;
                LogSemanticError(globals, "TODO 2"); // TODO
            } else if (exp1.at(aType) != tInt) {
                forAct[aType] = tError;
                LogSemanticError(globals, "TODO 3"); // TODO
            } else {
                forAct[aType] = tOk;
            }
        }

        // ------ //

        break;
    }

    // Como FORACT -> lambda, Follow (FORACT)
    case PARENTHESIS_CLOSE:
    case SEMICOLON: {
        WriteParse(output, globals, 27);

        // ------ //

        if (globals.useSemantic) {
            forAct[aType] = tOk;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Acción incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return forAct;
}

// HECHO
RuleAttributes Parser::Ass(std::ostream& output, GlobalState& globals) {
    RuleAttributes ass = {};

    // ASS -> = | +=
    switch (m_lastToken.type) {
    case ASSIGN: {
        WriteParse(output, globals, 28);

        // ------ //

        GetNextToken(globals);

        if (globals.useSemantic) {
            ass[aSum] = false;
        }

        // ------ //

        break;
    }

    case CUMULATIVE_ASSIGN: {
        WriteParse(output, globals, 29);

        // ------ //

        GetNextToken(globals);

        if (globals.useSemantic) {
            ass[aSum] = true;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            "Asignación incorrecta: "
            "Se esperaba «=» o «+=»."
        );
    }

    return ass;
}

// HECHO
RuleAttributes Parser::CallParams(std::ostream& output, GlobalState& globals) {
    RuleAttributes callParams = {};

    // CALLPARAMS -> EXP1 NEXTPARAMS | lambda
    switch (m_lastToken.type) {
    // First (EXP1 NEXTPARAMS)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER: {
        WriteParse(output, globals, 30);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        // ------ //

        const auto nextParams = NextParams(output, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) == tError || nextParams.at(aType) == tError) {
                callParams[aType] = tError;
            } else if (nextParams.at(aType) != tVoid) {
                callParams[aType] = exp1.at(aType) * nextParams.at(aType);
            } else {
                callParams[aType] = exp1.at(aType);
            }
        }

        // ------ //

        break;
    }

    // Como CALLPARAMS -> lambda, Follow (CALLPARAMS)
    case PARENTHESIS_CLOSE: {
        WriteParse(output, globals, 31);

        // ------ //

        if (globals.useSemantic) {
            callParams[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Lista de parámetros incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return callParams;
}

// HECHO
RuleAttributes Parser::NextParams(std::ostream& output, GlobalState& globals) {
    RuleAttributes nextParams = {};

    // NEXTPARAMS -> , EXP1 NEXTPARAMS | lambda
    switch (m_lastToken.type) {
    // First (EXP1 NEXTPARAMS)
    case COMMA: {
        WriteParse(output, globals, 32);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        // ------ //

        const auto nextParams_1 = NextParams(output, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) == tError || nextParams_1.at(aType) == tError) {
                nextParams[aType] = tError;
            } else if (nextParams_1.at(aType) != tVoid) {
                nextParams[aType] = exp1.at(aType) * nextParams_1.at(aType);
            } else {
                nextParams[aType] = exp1.at(aType);
            }
        }

        // ------ //

        break;
    }

    // Como NEXTPARAMS -> lambda, Follow (NEXTPARAMS)
    case PARENTHESIS_CLOSE: {
        WriteParse(output, globals, 33);

        // ------ //

        if (globals.useSemantic) {
            nextParams[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Lista de parámetros incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return nextParams;
}

// HECHO
RuleAttributes Parser::ReturnExp(std::ostream& output, GlobalState& globals) {
    RuleAttributes returnExp = {};

    // RETURNEXP -> EXP1 | lambda
    switch (m_lastToken.type) {
    // First (EXP1)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case TRUE:
    case FALSE:
    case IDENTIFIER: {
        WriteParse(output, globals, 34);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            returnExp[aType] = exp1.at(aType);
        }

        // ------ //

        break;
    }

    // Como RETURNEXP -> lambda, Follow (RETURNEXP)
    case SEMICOLON: {
        WriteParse(output, globals, 35);

        // ------ //

        if (globals.useSemantic) {
            returnExp[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión de retorno incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return returnExp;
}

// HECHO
RuleAttributes Parser::Exp1(std::ostream& output, GlobalState& globals) {
    RuleAttributes exp1 = {};

    // EXP1 -> EXP2 EXPOR
    switch (m_lastToken.type) {
    // First ( EXP2 EXPOR )
    case PARENTHESIS_OPEN:
    case CSTR:
    case CINT:
    case FALSE:
    case TRUE:
    case IDENTIFIER: {
        WriteParse(output, globals, 36);

        // ------ //

        const auto exp2 = Exp2(output, globals);

        // ------ //

        const auto expOr = ExpOr(output, globals);

        if (globals.useSemantic) {
            if (expOr.at(aType) == tVoid) {
                exp1[aType] = exp2.at(aType);
            } else if (exp2.at(aType) != tLog) {
                exp1[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (expOr.at(aType) == tError) {
                exp1[aType] = tError;
            } else {
                exp1[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return exp1;
}

// HECHO
RuleAttributes Parser::ExpOr(std::ostream& output, GlobalState& globals) {
    RuleAttributes expOr = {};

    // EXPOR -> || EXP2 EXPOR | lambda
    switch (m_lastToken.type) {
    // First ( || EXP2 EXPOR )
    case OR: {
        WriteParse(output, globals, 37);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto exp2 = Exp2(output, globals);

        // ------ //

        const auto expOr_1 = ExpOr(output, globals);

        if (globals.useSemantic) {
            if (exp2.at(aType) != tLog) {
                expOr[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (expOr_1.at(aType) == tError) {
                expOr[aType] = tError;
            } else {
                expOr[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    // Como EXPOR -> lambda, Follow (EXPOR)
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON: {
        WriteParse(output, globals, 38);

        // ------ //

        if (globals.useSemantic) {
            expOr[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return expOr;
}

// HECHO
RuleAttributes Parser::Exp2(std::ostream& output, GlobalState& globals) {
    RuleAttributes exp2 = {};

    // EXP2 -> EXP3 EXPAND
    switch (m_lastToken.type) {
    // First (EXP3 EXPAND)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER: {
        WriteParse(output, globals, 39);

        // ------ //

        const auto exp3 = Exp3(output, globals);

        // ------ //

        const auto expAnd = ExpAnd(output, globals);

        if (globals.useSemantic) {
            if (expAnd.at(aType) == tVoid) {
                exp2[aType] = exp3.at(aType);
            } else if (exp3.at(aType) != tLog) {
                exp2[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (expAnd.at(aType) == tError) {
                exp2[aType] == tError;
            } else {
                exp2[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return exp2;
}

// HECHO
RuleAttributes Parser::ExpAnd(std::ostream& output, GlobalState& globals) {
    RuleAttributes expAnd = {};

    // EXPAND -> && EXP3 EXPAND | lambda
    switch (m_lastToken.type) {
    // First ( && EXP3 EXPAND )
    case AND: {
        WriteParse(output, globals, 40);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto exp3 = Exp3(output, globals);

        // ------ //

        const auto expAnd_1 = ExpAnd(output, globals);

        if (globals.useSemantic) {
            if (exp3.at(aType) != tLog) {
                expAnd[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (expAnd_1.at(aType) == tError) {
                expAnd[aType] = tError;
            } else {
                expAnd[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    // Como EXPAND -> lambda, Follow (EXPAND)
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
    case OR: {
        WriteParse(output, globals, 41);

        // ------ //

        if (globals.useSemantic) {
            expAnd[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Sergio cambia esto."
            )
        );
    }

    return expAnd;
}

// HECHO
RuleAttributes Parser::Exp3(std::ostream& output, GlobalState& globals) {
    RuleAttributes exp3 = {};

    // EXP3 -> EXP4 COMP
    switch (m_lastToken.type) {
    // First (EXP4 COMP)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER: {
        WriteParse(output, globals, 42);

        // ------ //

        const auto exp4 = Exp4(output, globals);

        // ------ //

        const auto comp = Comp(output, globals);

        if (globals.useSemantic) {
            if (comp.at(aType) == tVoid) {
                exp3[aType] = exp4.at(aType);
            } else if (exp4.at(aType) != tInt) {
                exp3[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (comp.at(aType) == tError) {
                exp3[aType] = tError;
            } else {
                exp3[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return exp3;
}

// HECHO
RuleAttributes Parser::Comp(std::ostream& output, GlobalState& globals) {
    RuleAttributes comp = {};

    // COMP -> COMPOP EXP4 COMP | lambda
    switch (m_lastToken.type) {
    // First (COMPOP EXP4 COMP)
    case GREATER:
    case LESS: {
        WriteParse(output, globals, 43);

        // ------ //

        const auto compOp = CompOp(output, globals);

        // ------ //

        const auto exp4 = Exp4(output, globals);

        // ------ //

        const auto comp_1 = Comp(output, globals);

        // ------ //

        if (globals.useSemantic) {
            if (exp4.at(aType) != tInt) {
                comp[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (comp_1.at(aType) == tError) {
                comp[aType] = tError;
            } else {
                comp[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    // Como COMP -> lambda, Follow (COMP)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON: {
        WriteParse(output, globals, 44);

        // ------ //

        if (globals.useSemantic) {
            comp[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return comp;
}

// HECHO
RuleAttributes Parser::CompOp(std::ostream& output, GlobalState& globals) {
    RuleAttributes parser = {};

    // COMPOP -> > | <
    switch (m_lastToken.type) {
    // First (>)
    case GREATER: {
        WriteParse(output, globals, 45);

        // ------ //

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (<)
    case LESS: {
        WriteParse(output, globals, 46);

        // ------ //

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Comparación incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return parser;
}

// HECHO
RuleAttributes Parser::Exp4(std::ostream& output, GlobalState& globals) {
    RuleAttributes exp4 = {};

    // EXP4 -> EXPATOM ARITH
    switch (m_lastToken.type) {
    // First (EXPATOM ARITH)
    case PARENTHESIS_OPEN:
    case CINT:
    case CSTR:
    case FALSE:
    case TRUE:
    case IDENTIFIER: {
        WriteParse(output, globals, 47);

        // ------ //

        const auto expAtom = ExpAtom(output, globals);

        // ------ //

        const auto arith = Arith(output, globals);

        if (globals.useSemantic) {
            if (arith.at(aType) == tVoid) {
                exp4[aType] = expAtom.at(aType);
            } else if (expAtom.at(aType) != tInt) {
                exp4[aType] = tError;
            } else if (arith.at(aType) == tError) {
                exp4[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else {
                exp4[aType] = tInt;
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return exp4;
}

// HECHO
RuleAttributes Parser::Arith(std::ostream& output, GlobalState& globals) {
    RuleAttributes arith = {};

    // ARITH -> ARITHOP EXPATOM ARITH | lambda
    switch (m_lastToken.type) {
    // First (ARITHOP EXPATOM ARITH)
    case SUM:
    case SUB: {
        WriteParse(output, globals, 48);

        // ------ //

        const auto arithOp = ArithOp(output, globals);

        // ------ //

        const auto expAtom = ExpAtom(output, globals);

        // ------ //

        const auto arith_1 = Arith(output, globals);

        if (globals.useSemantic) {
            if (expAtom.at(aType) != tInt) {
                arith[aType] = tError;
                LogSemanticError(globals, "TODO"); // TODO
            } else if (arith_1.at(aType) == tError) {
                arith[aType] = tError;
            } else {
                arith[aType] = tInt;
            }
        }

        // ------ //

        break;
    }

    // Como ARITH -> lambda, Follow (ARITH)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case COMMA:
    case SEMICOLON:
    case LESS:
    case GREATER: {
        WriteParse(output, globals, 49);

        // ------ //

        if (globals.useSemantic) {
            arith[aType] = tVoid;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return arith;
}

// HECHO
RuleAttributes Parser::ArithOp(std::ostream& output, GlobalState& globals) {
    RuleAttributes arithOp = {};

    // ARITHOP -> + | -
    switch (m_lastToken.type) {
    // First (+)
    case SUM: {
        WriteParse(output, globals, 50);

        // ------ //

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (-)
    case SUB: {
        WriteParse(output, globals, 51);

        // ------ //

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Operación aritmética incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return arithOp;
}

// HECHO
RuleAttributes Parser::ExpAtom(std::ostream& output, GlobalState& globals) {
    RuleAttributes expAtom = {};

    // EXPATOM -> id IDVAL | ( EXP1 ) | cint | cstr | true | false
    switch (m_lastToken.type) {
    // First (id IDVAL)
    case IDENTIFIER: {
        WriteParse(output, globals, 52);

        // ------ //

        const auto id = m_lastToken;

        GetNextToken(globals);

        // ------ //

        const auto idVal = IdVal(output, globals);

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            if (idVal.at(aFunCall)) {
                const auto a_ = GetFunctionArgsType(type);

                if (!IsFunction(type)) {
                    expAtom[aType] = tError;
                    LogSemanticError(globals, "TODO: NO SE PUEDE LLAMAR USAR UNA VARIABLE COMO FUNCIÓN"); // TODO
                } else if (idVal.at(aType) == tError) {
                    expAtom[aType] = tError;
                } else if (idVal.at(aType) == GetFunctionArgsType(type)) {
                    expAtom[aType] = GetFunctionReturnType(type);
                } else {
                    expAtom[aType] = tError;
                    LogSemanticError(globals, "TODO: NO COINCIDEN LOS TIPOS EN LA LLAMADA A LA FUNCIÓN"); // TODO
                }
            } else {
                if (IsFunction(type)) {
                    expAtom[aType] = tError;
                    LogSemanticError(globals, "TODO: NO SE PUEDE USAR UNA FUNCIÓN COMO VARIABLE"); // TODO
                } else if (type == tInt) {
                    expAtom[aType] = tInt;
                } else if (type == tStr) {
                    expAtom[aType] = tStr;
                } else if (type == tLog) {
                    expAtom[aType] = tLog;
                } else {
                    expAtom[aType] = tError;
                    LogSemanticError(globals, "TODO: TIPO DE VARIABLE ILEGAL"); // TODO
                }
            }
        }

        // ------ //

        break;
    }

    // First (( EXP1 ))
    case PARENTHESIS_OPEN: {
        WriteParse(output, globals, 53);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            expAtom[aType] = exp1.at(aType);
        }

        // ------ //

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Expresión incorrecta: "
            "Falta el paréntesis de cierre."
        );

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (cint)
    case CINT: {
        WriteParse(output, globals, 54);

        // ------ //

        if (globals.useSemantic) {
            expAtom[aType] = tInt;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (cstr)
    case CSTR: {
        WriteParse(output, globals, 55);

        // ------ //

        if (globals.useSemantic) {
            expAtom[aType] = tStr;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (true)
    case TRUE: {
        WriteParse(output, globals, 56);

        // ------ //

        if (globals.useSemantic) {
            expAtom[aType] = tLog;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (false)
    case FALSE: {
        WriteParse(output, globals, 57);

        // ------ //

        if (globals.useSemantic) {
            expAtom[aType] = tLog;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Elemento de tipo «{}» inesperado.",
                ToString(m_lastToken.type)
            )
        );
    }

    return expAtom;
}

// HECHO
RuleAttributes Parser::IdVal(std::ostream& output, GlobalState& globals) {
    RuleAttributes idVal = {};

    // IDVAL -> ( CALLPARAMS ) | lambda
    switch (m_lastToken.type) {
    // First (( CALLPARAMS ))
    case PARENTHESIS_OPEN: {
        WriteParse(output, globals, 58);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto callParams = CallParams(output, globals);

        if (globals.useSemantic) {
            idVal[aFunCall] = true;
            idVal[aType] = callParams.at(aType);
        }

        // ------ //

        VerifyTokenType(
            PARENTHESIS_CLOSE,
            "Expresión incorrecta: "
            "Falta el paréntesis de cierre."
        );

        GetNextToken(globals);

        // ------ //

        break;
    }

    // Como IDVAL -> lambda, Follow (IDVAL)
    case AND:
    case OR:
    case PARENTHESIS_CLOSE:
    case SUM:
    case SUB:
    case COMMA:
    case SEMICOLON:
    case GREATER:
    case LESS: {
        WriteParse(output, globals, 59);

        // ------ //

        if (globals.useSemantic) {
            idVal[aFunCall] = false;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(
            std::format(
                "Expresión incorrecta: "
                "Se esperaba alguna acción sobre el identificador.",
                ToString(m_lastToken.type)
            )
        );
    }

    return idVal;
}

// HECHO
void Parser::Parse(std::ostream& output, GlobalState& globals) {
    try {
        globals.errorManager.SetLexicalRecoveryMode(LexicalRecoveryMode::SkipChar);

        globals.globalTable = SymbolTable(0);
        globals.tableCounter = 1;
        globals.globalOffset = 0;
        globals.implicitDeclaration = true;

        GetNextToken(globals);

        (void) Axiom(output, globals);
    } catch (const SyntaxException& e) {
        globals.errorManager.ProcessSyntaxException(*this, e);
    }
}

#pragma clang diagnostic pop
