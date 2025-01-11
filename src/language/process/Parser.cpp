#include "Parser.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "misc-no-recursion"
#pragma clang diagnostic ignored "bugprone-branch-clone"

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

        ValueProduct res = {};

        if (values.size() == 2) {
            res.Append(tVoid);
        } else {
            for (size_t i = 2; i < values.size(); i++) {
                res.Append(values[i]);
            }
        }

        return res;
    }
}

using enum TokenType;

Parser::Attributes Parser::Axiom(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef axiom = CreateRuleAttributes();

    // P -> FUNCTION P | STATEMENT P | eof
    switch (m_currentToken.type) {
    // First (FUNCTION P)
    case FUNCTION: {
        WriteParse(output, globals, 1);

        // ------ //

        const auto function = Function(output, globals);

        // ------ //

        const auto axiom_1 = Axiom(output, globals);

        if (globals.useSemantic) {
            if (function.at(aType) == tError || axiom_1.at(aType) == tError) {
                axiom[aType] = tError;
            } else {
                axiom[aType] = tOk;
            }
        }

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

        const auto statement = Statement(output, globals);

        // ------ //

        const auto axiom_1 = Axiom(output, globals);

        if (globals.useSemantic) {
            if (statement.at(aType) == tError || axiom_1.at(aType) == tError) {
                axiom[aType] = tError;
            } else {
                axiom[aType] = tOk;
            }
        }

        // ------ //

        break;
    }

    // First (eof)
    case END: {
        WriteParse(output, globals, 3);

        // ------ //

        if (globals.useSemantic) {
            axiom[aType] = tOk;
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::TOP_LEVEL_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Function(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef function = CreateRuleAttributes();

    // FUNCTION -> function FUNTYPE id ( FUNATTRIBUTES ) { BODY }

    assert(m_currentToken.type == FUNCTION);
    WriteParse(output, globals, 4);

    // ------ //

    if (globals.useSemantic) {
        globals.implicitDeclaration = false;
    }

    GetNextToken(globals);

    // ------ //

    const auto funType = FunType(output, globals);

    // ------ //

    VerifyTokenType(IDENTIFIER, SyntaxError::FUNCTION_MISSING_IDENTIFIER);

    const auto id = m_currentToken;

    if (globals.useSemantic) {
        globals.localTable = SymbolTable(globals.tableCounter);
        globals.tableCounter += 1;
        globals.localOffset = 0;
    }

    GetNextToken(globals);

    // ------ //

    VerifyTokenType(PARENTHESIS_OPEN, SyntaxError::FUNCTION_MISSING_PAREN_OPEN);

    GetNextToken(globals);

    // ------ //

    const auto funAttributes = FunAttributes(output, globals);

    if (globals.useSemantic) {
        const auto& pos = std::get<SymbolPos>(id.attribute);

        if (globals.HasType(pos)) {
            function[aType] = tError;
            LogSemanticError(
                globals,
                id,
                SemanticError::IDENTIFIER_ALREADY_EXISTS,
                "El nombre de la función ya está en uso."
            );
        } else {
            function[aType] = funAttributes.at(aType);
            globals.AddFunctionType(pos, funType.at(aType), funAttributes.at(aValueType));
            globals.AddFunctionTag(pos);
        }

        globals.implicitDeclaration = true;
    }

    // ------ //

    VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::FUNCTION_MISSING_PAREN_CLOSE);

    GetNextToken(globals);

    // ------ //

    VerifyTokenType(CURLY_BRACKET_OPEN, SyntaxError::FUNCTION_MISSING_BRACK_OPEN);

    GetNextToken(globals);

    // ------ //

    const auto body = Body(output, globals);

    // ------ //

    VerifyTokenType(CURLY_BRACKET_CLOSE, SyntaxError::FUNCTION_MISSING_BRACK_CLOSE);

    if (globals.useSemantic) {
        if (funType.at(aType) != body.at(aValueType)) {
            function[aType] = tError;

            const auto& pos = std::get<SymbolPos>(id.attribute);
            LogSemanticError(
                globals,
                funType,
                SemanticError::INVALID_RETURN_TYPE,
                std::format(
                    "El tipo de retorno de la función «{}» («{}») no coincide con el "
                    "devuelto («{}»).",
                    globals.GetSymbolName(pos),
                    funType.at(aType).ToReadableString(),
                    body.at(aValueType).ToReadableString()
                )
            );
        }

        WriteCurrentTable(output, globals);
        globals.localTable = std::nullopt;
    }

    GetNextToken(globals);

    // ------ //

    return PopCurrentAttributes();
}

Parser::Attributes Parser::FunType(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef funType = CreateRuleAttributes();

    // FUNTYPE -> RuleAttributesPos | VARTYPE
    switch (m_currentToken.type) {
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
        ThrowSyntaxError(SyntaxError::FUNTYPE_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::VarType(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef varType = CreateRuleAttributes();

    // VARTYPE -> int | boolean | string
    switch (m_currentToken.type) {
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
        ThrowSyntaxError(SyntaxError::VARTYPE_VOID);

    default:
        ThrowSyntaxError(SyntaxError::VARTYPE_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::FunAttributes(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef funAttributes = CreateRuleAttributes();

    // FUNATTRIBUTES -> RuleAttributesPos | VARTYPE id NEXTATTRIBUTES
    switch (m_currentToken.type) {
    // First (void)
    case VOID: {
        WriteParse(output, globals, 10);

        // ------ //

        if (globals.useSemantic) {
            funAttributes[aType] = tOk;
            funAttributes[aValueType] = tVoid;
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

        VerifyTokenType(IDENTIFIER, SyntaxError::FUNATTRIBUTES_MISSING_IDENTIFIER);

        const auto id = m_currentToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            if (globals.HasType(pos)) {
                funAttributes[aType] = tError;
                LogSemanticError(
                    globals,
                    id,
                    SemanticError::IDENTIFIER_ALREADY_EXISTS,
                    "El nombre de este atributo ya está en uso."
                );
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
            if (nextAttributes.at(aType) == tError) {
                funAttributes[aType] = tError;
            }

            if (nextAttributes.at(aValueType) != tVoid) {
                funAttributes[aValueType] = varType.at(aType) * nextAttributes.at(aValueType);
            } else {
                funAttributes[aValueType] = varType.at(aType);
            }
        }

        // ------ //

        break;
    }

    case PARENTHESIS_CLOSE:
        ThrowSyntaxError(SyntaxError::FUNATTRIBUTES_EMPTY);

    default:
        ThrowSyntaxError(SyntaxError::FUNATTRIBUTES_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::NextAttributes(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef nextAttributes = CreateRuleAttributes();

    // NEXTATTRIBUTES -> , VARTYPE id NEXTATTRIBUTES | lambda
    switch (m_currentToken.type) {
    // First (, VARTYPE id NEXTATTRIBUTES)
    case COMMA: {
        WriteParse(output, globals, 12);

        // ------ //

        GetNextToken(globals);

        // ------ //

        const auto varType = VarType(output, globals);

        // ------ //

        VerifyTokenType(IDENTIFIER, SyntaxError::FUNATTRIBUTES_MISSING_IDENTIFIER);

        const auto id = m_currentToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            if (globals.HasType(pos)) {
                nextAttributes[aType] = tError;
                LogSemanticError(
                    globals,
                    id,
                    SemanticError::IDENTIFIER_ALREADY_EXISTS,
                    "El nombre de este atributo ya está en uso."
                );
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
            if (nextAttributes.at(aType) == tError) {
                nextAttributes[aType] = tError;
            }

            if (nextAttributes_1.at(aValueType) != tVoid) {
                nextAttributes[aValueType] = varType.at(aType) * nextAttributes_1.at(aValueType);
            } else {
                nextAttributes[aValueType] = varType.at(aType);
            }
        }

        // ------ //

        break;
    }

    // Como NEXTATTRIBUTES -> lambda, Follow (NEXTATTRIBUTES)
    default: {
        WriteParse(output, globals, 13);

        // ------ //

        if (globals.useSemantic) {
            nextAttributes[aType] = tOk;
            nextAttributes[aValueType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Body(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef body = CreateRuleAttributes();

    // BODY -> STATEMENT BODY | lambda
    switch (m_currentToken.type) {
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

            if (statement.at(aValueType) == body_1.at(aValueType) || statement.at(aValueType) == tVoid) {
                body[aValueType] = body_1.at(aValueType);
            } else if (body_1.at(aValueType) == tVoid) {
                body[aValueType] = statement.at(aValueType);
            } else {
                body[aValueType] = tError;
            }
        }

        // ------ //

        break;
    }

    // Como BODY -> lambda, Follow (BODY)
    default: {
        WriteParse(output, globals, 15);

        // ------ //

        if (globals.useSemantic) {
            body[aType] = tOk;
            body[aValueType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Statement(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef statement = CreateRuleAttributes();

    // STATEMENT -> if ( EXP1 ) ATOMSTATEMENT | for ( FORACT ; EXP1 ; FORACT ) { BODY } |
    //              var VARTYPE id ; | ATOMSTATEMENT
    switch (m_currentToken.type) {
    // First (if ( EXP1 ) ATOMSTATEMENT)
    case IF: {
        WriteParse(output, globals, 16);

        // ------ //

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(PARENTHESIS_OPEN, SyntaxError::STATEMENT_IF_MISSING_PAREN_OPEN);

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) == tError) {
                statement[aType] = tError;
            } else if (exp1.at(aType) != tLog) {
                LogSemanticError(
                    globals,
                    exp1,
                    SemanticError::INVALID_IF_CONDITION_TYPE,
                    std::format(
                        "El argumento de «if» debe ser de tipo «boolean». El tipo "
                        "evaluado es «{}».",
                        exp1.at(aType).ToReadableString()
                    )
                );
                statement[aType] = tError;
            } else {
                statement[aType] = tOk;
            }
        }

        // ------ //

        VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::STATEMENT_IF_MISSING_PAREN_CLOSE);

        GetNextToken(globals);

        // ------ //

        const auto atomStatement = AtomStatement(output, globals);

        if (globals.useSemantic) {
            if (statement.at(aType) != tError) {
                statement[aType] = atomStatement.at(aType);
            }

            statement[aValueType] = atomStatement.at(aValueType);
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

        VerifyTokenType(PARENTHESIS_OPEN, SyntaxError::STATEMENT_FOR_MISSING_PAREN_OPEN);

        GetNextToken(globals);

        // ------ //

        const auto forAct_1 = ForAct(output, globals);

        if (globals.useSemantic) {
            if (forAct_1.at(aType) == tError) {
                statement[aType] = tError;
            } else {
                statement[aType] = tOk;
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_FOR_MISSING_SEMICOLON);

        GetNextToken(globals);

        // ------ //

        const auto exp1 = Exp1(output, globals);

        if (globals.useSemantic) {
            if (exp1.at(aType) == tError) {
                statement[aType] = tError;
            } else if (exp1.at(aType) != tLog) {
                statement[aType] = tError;
                LogSemanticError(
                    globals,
                    exp1,
                    SemanticError::INVALID_FOR_CONDITION_TYPE,
                    std::format(
                        "El segundo argumento de «for» debe ser de tipo «boolean». El tipo "
                        "evaluado es «{}».",
                        exp1.at(aType).ToReadableString()
                    )
                );
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_FOR_MISSING_SEMICOLON);

        GetNextToken(globals);

        // ------ //

        const auto forAct_2 = ForAct(output, globals);

        if (globals.useSemantic) {
            if (forAct_2.at(aType) == tError) {
                statement[aType] = tError;
            }
        }

        // ------ //

        VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::STATEMENT_FOR_MISSING_PAREN_CLOSE);

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(CURLY_BRACKET_OPEN, SyntaxError::STATEMENT_FOR_MISSING_BRACK_OPEN);

        GetNextToken(globals);

        // ------ //

        const auto body = Body(output, globals);

        // ------ //

        VerifyTokenType(CURLY_BRACKET_CLOSE, SyntaxError::STATEMENT_FOR_MISSING_BRACK_CLOSE);

        if (globals.useSemantic) {
            statement[aType] = body.at(aType);
            statement[aValueType] = body.at(aValueType);
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (var VARTYPE id ;)
    case VAR: {
        WriteParse(output, globals, 18);

        // ------ //

        GetNextToken(globals);

        if (globals.useSemantic) {
            globals.implicitDeclaration = false;
        }

        // ------ //

        const auto varType = VarType(output, globals);

        // ------ //

        VerifyTokenType(IDENTIFIER, SyntaxError::STATEMENT_VAR_MISSING_IDENTIFIER);

        const auto id = m_currentToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);

            statement[aValueType] = tVoid;

            if (globals.HasType(pos)) {
                statement[aType] = tError;
                LogSemanticError(
                    globals,
                    id,
                    SemanticError::IDENTIFIER_ALREADY_EXISTS,
                    "El nombre de la variable a declarar ya está en uso."
                );
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

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_MISSING_END_SEMICOLON);

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
            statement[aValueType] = atomStatement.at(aValueType);
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::STATEMENT_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::AtomStatement(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef atomStatement = CreateRuleAttributes();

    // ATOMSTATEMENT -> id IDACT ; | output EXP1 ; | input id ; | return RETURNEXP ;
    switch (m_currentToken.type) {
    // First (id IDACT ;)
    case IDENTIFIER: {
        WriteParse(output, globals, 20);

        // ------ //

        const auto id = m_currentToken;

        GetNextToken(globals);

        // ------ //

        const auto idAct = IdAct(output, globals);

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            atomStatement[aValueType] = tVoid;

            if (idAct.at(aFunCall)) {
                if (!IsFunction(type)) {
                    atomStatement[aType] = tError;
                    LogSemanticError(
                        globals,
                        atomStatement,
                        SemanticError::INCOHERENT_VARIABLE_AS_FUNCTION_TYPES,
                        "No se puede llamar a una variable como si fuera una función."
                    );
                } else if (idAct.at(aType) == tError) {
                    atomStatement[aType] = tError;
                } else if (idAct.at(aType) == GetFunctionArgsType(type)) {
                    atomStatement[aType] = tOk;
                } else {
                    atomStatement[aType] = tError;
                    LogSemanticError(
                        globals,
                        idAct,
                        SemanticError::INCOHERENT_CALL_PARAM_TYPES,
                        std::format(
                            "Los tipos de los argumentos de llamada a la función no "
                            "coinciden con los de la definición. Se esperaba «{}», "
                            "pero se recibió «{}».",
                            GetFunctionArgsType(type).ToReadableString(),
                            idAct.at(aType).ToReadableString()
                        )
                    );
                }
            } else {
                if (IsFunction(type)) {
                    atomStatement[aType] = tError;
                    LogSemanticError(
                        globals,
                        idAct,
                        SemanticError::INCOHERENT_FUNCTION_AS_VARIABLE_TYPES,
                        "No se puede asignar valores a una función."
                    );
                } else if (idAct.at(aType) == tError) {
                    atomStatement[aType] = tError;
                } else if (idAct.at(aType) == type) {
                    atomStatement[aType] = tOk;
                } else {
                    atomStatement[aType] = tError;
                    LogSemanticError(
                        globals,
                        idAct,
                        SemanticError::INCOHERENT_TYPES,
                        std::format(
                            "Los tipos a ambos lados de la asignación no coinciden. "
                            "La variable es de tipo «{}», pero la expresión es «{}».",
                            type.ToReadableString(),
                            idAct.at(aType).ToReadableString()
                        )
                    );
                }
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_MISSING_END_SEMICOLON);

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
            atomStatement[aValueType] = tVoid;

            if (exp1.at(aType) == tError) {
                atomStatement[aType] = tError;
            } else if (exp1.at(aType) == tStr || exp1.at(aType) == tInt) {
                atomStatement[aType] = tOk;
            } else {
                atomStatement[aType] = tError;
                LogSemanticError(
                    globals,
                    exp1,
                    SemanticError::INVALID_OUTPUT_TYPE,
                    std::format(
                        "Una expresión con tipo «{}» no se puede mostrar con «output». "
                        "«output» permite mostrar «int» y «string».",
                        exp1.at(aType).ToReadableString()
                    )
                );
            }
        }

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_MISSING_END_SEMICOLON);

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

        VerifyTokenType(IDENTIFIER, SyntaxError::STATEMENT_INPUT_MISSING_IDENTIFIER);

        const auto id = m_currentToken;

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            atomStatement[aValueType] = tVoid;

            if (type == tStr || type == tInt) {
                atomStatement[aType] = tOk;
            } else {
                atomStatement[aType] = tError;
                LogSemanticError(
                    globals,
                    id,
                    SemanticError::INVALID_INPUT_TYPE,
                    std::format(
                        "Un objeto tipo «{}» no se puede usar con «input». "
                        "«input» acepta variables de tipo «int» o «string».",
                        type.ToReadableString()
                    )
                );
            }
        }

        GetNextToken(globals);

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_MISSING_END_SEMICOLON);

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

            atomStatement[aValueType] = returnExp.at(aType);
        }

        // ------ //

        VerifyTokenType(SEMICOLON, SyntaxError::STATEMENT_MISSING_END_SEMICOLON);

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::STATEMENT_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::IdAct(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef idAct = CreateRuleAttributes();

    // IDACT -> ASS EXP1 | ( CALLPARAMS )
    switch (m_currentToken.type) {
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
                    LogSemanticError(
                        globals,
                        exp1,
                        SemanticError::INVALID_TYPE,
                        std::format(
                            "Sólo se puede incrementar con expresiones de tipo "
                            "«int» o concatenar con expresiones de tipo "
                            "«string». El tipo actual es {}.",
                            type.ToReadableString(),
                            idAct.at(aType).ToReadableString()
                        )
                    );
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

        VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::IDACT_CALL_MISSING_PAREN_CLOSE);

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::IDACT_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ForAct(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef forAct = CreateRuleAttributes();

    // FORACT -> id ASS EXP1 | lambda
    switch (m_currentToken.type) {
    // First (id ASS EXP1)
    case IDENTIFIER: {
        WriteParse(output, globals, 26);

        // ------ //

        const auto id = m_currentToken;

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
                LogSemanticError(
                    globals,
                    id,
                    SemanticError::INVALID_FOR_ACTION_TYPE,
                    std::format(
                        "El tipo de una variable en «for» debe de ser «int». El tipo actual es «{}».",
                        type.ToReadableString()
                    )
                );
            } else if (exp1.at(aType) == tError) {
                forAct[aType] = tError;
            } else if (exp1.at(aType) != tInt) {
                forAct[aType] = tError;
                LogSemanticError(
                    globals,
                    exp1,
                    SemanticError::INCOHERENT_TYPES,
                    std::format(
                        "Se esperaba una expresión con tipo «int». El tipo de la expresión es «{}».",
                        exp1.at(aType).ToReadableString()
                    )
                );
            } else {
                forAct[aType] = tOk;
            }
        }

        // ------ //

        break;
    }

    // Como FORACT -> lambda, Follow (FORACT)
    default: {
        WriteParse(output, globals, 27);

        // ------ //

        if (globals.useSemantic) {
            forAct[aType] = tOk;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Ass(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef ass = CreateRuleAttributes();

    // ASS -> = | +=
    switch (m_currentToken.type) {
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
        ThrowSyntaxError(SyntaxError::ASS_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::CallParams(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef callParams = CreateRuleAttributes();

    // CALLPARAMS -> EXP1 NEXTPARAMS | lambda
    switch (m_currentToken.type) {
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
    default: {
        WriteParse(output, globals, 31);

        // ------ //

        if (globals.useSemantic) {
            callParams[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::NextParams(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef nextParams = CreateRuleAttributes();

    // NEXTPARAMS -> , EXP1 NEXTPARAMS | lambda
    switch (m_currentToken.type) {
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
    default: {
        WriteParse(output, globals, 33);

        // ------ //

        if (globals.useSemantic) {
            nextParams[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ReturnExp(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef returnExp = CreateRuleAttributes();

    // RETURNEXP -> EXP1 | lambda
    switch (m_currentToken.type) {
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
    default: {
        WriteParse(output, globals, 35);

        // ------ //

        if (globals.useSemantic) {
            returnExp[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Exp1(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef exp1 = CreateRuleAttributes();

    // EXP1 -> EXP2 EXPOR
    switch (m_currentToken.type) {
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
            } else if (exp2.at(aType) == tError) {
                exp1[aType] = tError;
            } else if (exp2.at(aType) != tLog) {
                exp1[aType] = tError;
                LogSemanticError(
                    globals,
                    exp2,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Para poder aplicar un operador lógico, la expresión debe ser de tipo «boolean». "
                        "El tipo de la expresión es «{}».",
                        exp2.at(aType).ToReadableString()
                    )
                );
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
        ThrowSyntaxError(SyntaxError::EXP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ExpOr(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef expOr = CreateRuleAttributes();

    // EXPOR -> || EXP2 EXPOR | lambda
    switch (m_currentToken.type) {
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
            if (exp2.at(aType) == tError) {
                expOr[aType] = tError;
            } else if (exp2.at(aType) != tLog) {
                expOr[aType] = tError;
                LogSemanticError(
                    globals,
                    exp2,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Para poder aplicar un operador lógico, la expresión debe ser de tipo «boolean». "
                        "El tipo de la expresión es «{}».",
                        exp2.at(aType).ToReadableString()
                    )
                );
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
    default: {
        WriteParse(output, globals, 38);

        // ------ //

        if (globals.useSemantic) {
            expOr[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Exp2(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef exp2 = CreateRuleAttributes();

    // EXP2 -> EXP3 EXPAND
    switch (m_currentToken.type) {
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
            } else if (exp3.at(aType) == tError) {
                exp2[aType] = tError;
            } else if (exp3.at(aType) != tLog) {
                exp2[aType] = tError;
                LogSemanticError(
                    globals,
                    exp3,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Para poder aplicar un operador lógico, la expresión debe ser de tipo «boolean». "
                        "El tipo de la expresión es «{}».",
                        exp3.at(aType).ToReadableString()
                    )
                );
            } else if (expAnd.at(aType) == tError) {
                exp2[aType] = tError;
            } else {
                exp2[aType] = tLog;
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::EXP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ExpAnd(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef expAnd = CreateRuleAttributes();

    // EXPAND -> && EXP3 EXPAND | lambda
    switch (m_currentToken.type) {
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
            if (exp3.at(aType) == tError) {
                expAnd[aType] = tError;
            } else if (exp3.at(aType) != tLog) {
                expAnd[aType] = tError;
                LogSemanticError(
                    globals,
                    exp3,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Para poder aplicar un operador lógico, la expresión debe ser de tipo «boolean». "
                        "El tipo de la expresión es «{}».",
                        exp3.at(aType).ToReadableString()
                    )
                );
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
    default: {
        WriteParse(output, globals, 41);

        // ------ //

        if (globals.useSemantic) {
            expAnd[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Exp3(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef exp3 = CreateRuleAttributes();

    // EXP3 -> EXP4 COMP
    switch (m_currentToken.type) {
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
            } else if (exp4.at(aType) == tError) {
                exp3[aType] = tError;
            } else if (exp4.at(aType) != tInt) {
                exp3[aType] = tError;
                LogSemanticError(
                    globals,
                    exp4,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Sólo se pueden comparar valores de tipo «int». El tipo actual es «{}».",
                        exp4.at(aType).ToReadableString()
                    )
                );
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
        ThrowSyntaxError(SyntaxError::EXP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Comp(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef comp = CreateRuleAttributes();

    // COMP -> COMPOP EXP4 COMP | lambda
    switch (m_currentToken.type) {
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
            if (exp4.at(aType) == tError) {
                comp[aType] = tError;
            } else if (exp4.at(aType) != tInt) {
                comp[aType] = tError;
                LogSemanticError(
                    globals,
                    exp4,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Sólo se pueden comparar valores de tipo «int». El tipo actual es «{}».",
                        exp4.at(aType).ToReadableString()
                    )
                );
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
    default: {
        WriteParse(output, globals, 44);

        // ------ //

        if (globals.useSemantic) {
            comp[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::CompOp(std::ostream& output, GlobalState& globals) {
    [[maybe_unused]] const AttributesPosRef parser = CreateRuleAttributes();

    // COMPOP -> > | <
    switch (m_currentToken.type) {
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
        ThrowSyntaxError(SyntaxError::COMP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Exp4(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef exp4 = CreateRuleAttributes();

    // EXP4 -> EXPATOM ARITH
    switch (m_currentToken.type) {
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
            } else if (expAtom.at(aType) != tInt && expAtom.at(aType) != tStr) {
                exp4[aType] = tError;
                LogSemanticError(
                    globals,
                    expAtom,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "Una operación aritmética requiere que el tipo sea «int» o «string»."
                        " El tipo actual es «{}».",
                        expAtom.at(aType).ToReadableString()
                    )
                );
            } else if (arith.at(aType) == tError) {
                exp4[aType] = tError;
            } else if (expAtom.at(aType) != arith.at(aType)) {
                exp4[aType] = tError;
                LogSemanticError(
                    globals,
                    exp4,
                    SemanticError::INCOHERENT_TYPES,
                    std::format(
                        "Los tipos no concuerdan. Los tipos de las expresiones son «{}» y «{}».",
                        expAtom.at(aType).ToReadableString(),
                        arith.at(aType).ToReadableString()
                    )
                );
            } else {
                exp4[aType] = arith.at(aType);
            }
        }

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::EXP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::Arith(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef arith = CreateRuleAttributes();

    // ARITH -> ARITHOP EXPATOM ARITH | lambda
    switch (m_currentToken.type) {
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
            if (!arithOp.at(aSum) && expAtom.at(aType) != tInt) {
                arith[aType] = tError;
                LogSemanticError(
                    globals,
                    expAtom,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "La resta sólo se puede realizar sobre expresiones de "
                        "tipo «int», pero el tipo de la expresión es «{}».",
                        expAtom.at(aType).ToReadableString()
                    )
                );
            } else if (arithOp.at(aSum) && expAtom.at(aType) != tInt && expAtom.at(aType) != tStr) {
                arith[aType] = tError;
                LogSemanticError(
                    globals,
                    expAtom,
                    SemanticError::INVALID_TYPE,
                    std::format(
                        "El operador «+» sólo se puede aplicar a expresiones de tipo "
                        "«int» o «string», pero el tipo de la expresión es «{}».",
                        expAtom.at(aType).ToReadableString()
                    )
                );
            } else if (arith_1.at(aType) == tError) {
                arith[aType] = tError;
            } else if (arith_1.at(aType) != tVoid && expAtom.at(aType) != arith_1.at(aType)) {
                arith[aType] = tError;
                LogSemanticError(
                    globals,
                    expAtom,
                    arith_1,
                    SemanticError::INCOHERENT_TYPES,
                    std::format(
                        "Los tipos no concuerdan. Los tipos de las expresiones son «{}» y «{}».",
                        expAtom.at(aType).ToReadableString(),
                        arith_1.at(aType).ToReadableString()
                    )
                );
            } else {
                arith[aType] = expAtom.at(aType);
            }
        }

        // ------ //


        break;
    }

    // Como ARITH -> lambda, Follow (ARITH)
    default: {
        WriteParse(output, globals, 49);

        // ------ //

        if (globals.useSemantic) {
            arith[aType] = tVoid;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ArithOp(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef arithOp = CreateRuleAttributes();

    // ARITHOP -> + | -
    switch (m_currentToken.type) {
    // First (+)
    case SUM: {
        WriteParse(output, globals, 50);

        // ------ //

        if (globals.useSemantic) {
            arithOp[aSum] = true;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    // First (-)
    case SUB: {
        WriteParse(output, globals, 51);

        // ------ //

        if (globals.useSemantic) {
            arithOp[aSum] = false;
        }

        GetNextToken(globals);

        // ------ //

        break;
    }

    default:
        ThrowSyntaxError(SyntaxError::ARITH_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::ExpAtom(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef expAtom = CreateRuleAttributes();

    // EXPATOM -> id IDVAL | ( EXP1 ) | cint | cstr | true | false
    switch (m_currentToken.type) {
    // First (id IDVAL)
    case IDENTIFIER: {
        WriteParse(output, globals, 52);

        // ------ //

        const auto id = m_currentToken;

        GetNextToken(globals);

        // ------ //

        const auto idVal = IdVal(output, globals);

        if (globals.useSemantic) {
            const auto& pos = std::get<SymbolPos>(id.attribute);
            const auto type = globals.GetType(pos);

            if (idVal.at(aFunCall)) {
                if (!IsFunction(type)) {
                    expAtom[aType] = tError;
                    LogSemanticError(
                        globals,
                        expAtom,
                        SemanticError::INCOHERENT_VARIABLE_AS_FUNCTION_TYPES,
                        "No se puede llamar a una variable como si fuera una función."
                    );
                } else if (idVal.at(aType) == tError) {
                    expAtom[aType] = tError;
                } else if (idVal.at(aType) == GetFunctionArgsType(type)) {
                    expAtom[aType] = GetFunctionReturnType(type);
                } else {
                    expAtom[aType] = tError;
                    LogSemanticError(
                        globals,
                        idVal,
                        SemanticError::INCOHERENT_CALL_PARAM_TYPES,
                        std::format(
                            "Los tipos de los argumentos de llamada a la función no "
                            "coinciden con los de la definición. Se esperaba «{}», "
                            "pero se recibió «{}».",
                            GetFunctionArgsType(type).ToReadableString(),
                            idVal.at(aType).ToReadableString()
                        )
                    );
                }
            } else {
                if (IsFunction(type)) {
                    expAtom[aType] = tError;
                    LogSemanticError(
                        globals,
                        idVal,
                        SemanticError::INCOHERENT_FUNCTION_AS_VARIABLE_TYPES,
                        "No se puede asignar valores a una función."
                    );
                } else {
                    expAtom[aType] = type;
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

        VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::EXP_MISSING_PAREN_CLOSE);

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
        ThrowSyntaxError(SyntaxError::EXP_INVALID);
    }

    return PopCurrentAttributes();
}

Parser::Attributes Parser::IdVal(std::ostream& output, GlobalState& globals) {
    const AttributesPosRef idVal = CreateRuleAttributes();

    // IDVAL -> ( CALLPARAMS ) | lambda
    switch (m_currentToken.type) {
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

        VerifyTokenType(PARENTHESIS_CLOSE, SyntaxError::IDACT_CALL_MISSING_PAREN_CLOSE);

        GetNextToken(globals);

        // ------ //

        break;
    }

    // Como IDVAL -> lambda, Follow (IDVAL)
    default: {
        WriteParse(output, globals, 59);

        // ------ //

        if (globals.useSemantic) {
            idVal[aFunCall] = false;
        }

        // ------ //

        break;
    }
    }

    return PopCurrentAttributes();
}

void Parser::Parse(std::ostream& output, GlobalState& globals) {
    try {
        globals.errorManager.SetLexicalRecoveryMode(LexicalRecoveryMode::SkipChar);

        globals.globalTable = SymbolTable(0);
        globals.tableCounter = 1;
        globals.globalOffset = 0;
        globals.implicitDeclaration = true;

        GetNextToken(globals);

        const auto axiom = Axiom(output, globals);
        if (globals.useSemantic && axiom.at(aType) == tError) {
            assert(globals.errorManager.GetStatus() != 0);
            globals.errorManager.LogError("Hay errores semánticos al procesar el programa.");
        }
    } catch (const SyntaxException& e) {
        globals.errorManager.ProcessSyntaxException(m_lexer, e);
    }
}

#pragma clang diagnostic pop
