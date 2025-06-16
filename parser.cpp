#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

void Parser::expect(Token::Type type) {
    if (!match(type)) {
        cout << "Error: se esperaba el token " << type << ", pero se encontró '" << current->text << "' (tipo: " << current->type << ")" << endl;
        exit(1);
    }
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}

Stm* Parser::parseVarDec() {
    bool isVal = false;
    bool isVar = false;

    if (match(Token::VAR)) {
        isVar = true;
    } else {
        return nullptr;
    }

    if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador después de 'var'." << endl;
        exit(1);
    }
    string varname = previous->text;

    if (!match(Token::COLON)) {
        cout << "Error: se esperaba ':' después del identificador." << endl;
        exit(1);
    }

    string type;
    if (match(Token::TYPEINT)) {
        type = "Int";
    } else if (match(Token::TYPEFLOAT)) {
        type = "Float";
    } else if (match(Token::ID)) {
        type = previous->text;
    } else {
        cout << "Error: se esperaba un tipo después de ':'." << endl;
        exit(1);
    }

    Exp* init = nullptr;
    if (match(Token::ASSIGN)) {
        init = parseExpression();
    }

    // No consumir punto y coma aquí, se maneja en parseStatement

    // Si hay inicialización, crear VarDecWithInit
    if (init != nullptr) {
        return new VarDecWithInit(varname, type, init);
    }

    // Si no hay inicialización, crear VarDec tradicional
    list<string> ids = { varname };
    return new VarDec(type, ids);
}

VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    while (true) {
        Token* saved = current;

        Stm* aux = parseVarDec();
        if (aux == nullptr) {
            current = saved;
            break;
        }

        // Solo añadir VarDec sin inicialización a VarDecList
        if (auto* vd = dynamic_cast<VarDec*>(aux)) {
            vdl->add(vd);
        } else {
            // Si es VarDecWithInit, no pertenece a VarDecList
            // Restaurar el token y salir
            current = saved;
            delete aux;
            break;
        }
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    while (!check(Token::LLAVED) && !isAtEnd()) {
        // Saltar puntos y coma sueltos
        if (match(Token::PC)) {
            continue;
        }

        Stm* stmt = parseStatement();
        if (stmt != nullptr) {
            sl->add(stmt);
        } else {
            break;
        }
    }
    return sl;
}

Body* Parser::parseBody() {
    expect(Token::LLAVEI);

    VarDecList* vardecs = parseVarDecList();         // sin inicialización
    StatementList* slist = parseStatementList();     // con inicialización, prints, etc.

    expect(Token::LLAVED);

    return new Body(vardecs, slist);
}

Program* Parser::parseProgram() {
    VarDecList* vardecs = new VarDecList();
    auto fundecs = parseFunDecList();
    return new Program(vardecs, fundecs);
}

FunDecList* Parser::parseFunDecList() {
    FunDecList* vdl = new FunDecList();
    FunDec* aux;
    aux = parseFunDec();
    while (aux != NULL) {
        vdl->add(aux);
        aux = parseFunDec();
    }
    return vdl;
}

// Corregido para sintaxis de función Kotlin
FunDec* Parser::parseFunDec() {
    FunDec* fd = nullptr;

    if (match(Token::FUN)) {
        fd = new FunDec();

        if (!match(Token::ID)) {
            cout << "Error: se esperaba nombre de función después de 'fun', pero se encontró '" << current->text << "'" << endl;
            exit(1);
        }
        fd->nombre = previous->text;

        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después del nombre de la función, pero se encontró '" << current->text << "'" << endl;
            exit(1);
        }

        // Parseo de parámetros: nombre : tipo
        while (check(Token::ID)) {
            match(Token::ID);
            string paramName = previous->text;

            if (!match(Token::COLON)) {
                cout << "Error: se esperaba ':' después del nombre del parámetro '" << paramName << "', pero se encontró '" << current->text << "'" << endl;
                exit(1);
            }

            string paramType;
            if (match(Token::TYPEINT)) {
                paramType = "Int";
            } else if (match(Token::TYPEFLOAT)) {
                paramType = "Float";
            } else if (match(Token::ID)) {
                paramType = previous->text;
            } else {
                cout << "Error: se esperaba tipo del parámetro después de ':', pero se encontró '" << current->text << "'" << endl;
                exit(1);
            }

            fd->parametros.push_back(paramName);
            fd->tipos.push_back(paramType);

            if (!match(Token::COMA)) {
                break;
            }
        }

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' al final de los parámetros, pero se encontró '" << current->text << "'" << endl;
            exit(1);
        }

        // Manejo del tipo de retorno (opcional en Kotlin)
        if (match(Token::COLON)) {
            if (match(Token::TYPEINT)) {
                fd->tipo = "Int";
            } else if (match(Token::TYPEFLOAT)) {
                fd->tipo = "Float";
            } else if (match(Token::ID)) {
                fd->tipo = previous->text;
            } else {
                fd->tipo = "Unit"; // Tipo por defecto en Kotlin
            }
        } else {
            fd->tipo = "Unit"; // Tipo por defecto en Kotlin
        }

        fd->cuerpo = parseBody();
    }

    return fd;
}

list<Stm*> Parser::parseStmList() {
    list<Stm*> slist;
    slist.push_back(parseStatement());
    while(match(Token::PC)) {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;
    Body* tb = NULL;
    Body* fb = NULL;

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    // Intentar parsear declaración de variable con inicialización
    if (check(Token::VAR)) {
        s = parseVarDec();
        // Consumir punto y coma opcional
        match(Token::PC);
        return s;
    }
    else if (match(Token::ID)) {
        string lex = previous->text;

        if (match(Token::ASSIGN)) {
            e = parseCExp();
            s = new AssignStatement(lex, e);
            // Consumir punto y coma opcional
            match(Token::PC);
        }
        else if (match(Token::PI)) {
            // Llamada a función
            FCallExp* fcall = new FCallExp();
            fcall->nombre = lex;

            if (!check(Token::PD)) {
                fcall->argumentos.push_back(parseCExp());
                while (match(Token::COMA)) {
                    fcall->argumentos.push_back(parseCExp());
                }
            }

            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' después de los argumentos." << endl;
                exit(1);
            }

            // Usar PrintStatement para manejar llamadas a función
            s = new PrintStatement(fcall);
            // Consumir punto y coma opcional
            match(Token::PC);
        }
        else {
            return NULL;
        }
    }
    else if (match(Token::PRINT)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'print'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e, false); // false for print
        // Consumir punto y coma opcional
        match(Token::PC);
    }
    else if (match(Token::PRINTLN)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'println'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e, true); // true for println
        // Consumir punto y coma opcional
        match(Token::PC);
    }
    else if (match(Token::RETURN)) {
        ReturnStatement* rs = new ReturnStatement();
        // En Kotlin, return puede no tener paréntesis
        if (!check(Token::PC) && !check(Token::LLAVED) && !check(Token::END) && !isAtEnd()) {
            rs->e = parseCExp();
        }
        s = rs;
        // Consumir punto y coma opcional
        match(Token::PC);
    }
    else if (match(Token::IF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'if'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la condición del if." << endl;
            exit(1);
        }
        tb = parseBody();
        if (match(Token::ELSE)) {
            fb = parseBody();
        }
        s = new IfStatement(e, tb, fb);
        // No necesita punto y coma después de estructura de control
    }
    else if (match(Token::WHILE)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'while'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la condición del while." << endl;
            exit(1);
        }
        tb = parseBody();
        s = new WhileStatement(e, tb);
        // No necesita punto y coma después de estructura de control
    }
    else {
        return NULL;
    }

    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ) || match(Token::GT) || match(Token::GE)){
        BinaryOp op;
        if (previous->type == Token::LT){
            op = LT_OP;
        }
        else if (previous->type == Token::LE){
            op = LE_OP;
        }
        else if (previous->type == Token::EQ){
            op = EQ_OP;
        }
        else if (previous->type == Token::GT){
            op = GT_OP;
        }
        else if (previous->type == Token::GE){
            op = GE_OP;
        }
        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS){
            op = MINUS_OP;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV){
            op = DIV_OP;
        }
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    Exp* e;
    Exp* e1;
    Exp* e2;
    if (match(Token::TRUE)){
        return new BoolExp(1);
    }else if (match(Token::FALSE)){
        return new BoolExp(0);
    }
    else if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::FLOATNUM)) {
        string floatStr = previous->text;
        // Remove 'f' suffix if present
        if (floatStr.back() == 'f' || floatStr.back() == 'F') {
            floatStr.pop_back();
        }
        return new FloatExp(stof(floatStr));
    }
    else if (match(Token::ID)) {
        string name = previous->text;

        if (match(Token::PI)) {
            FCallExp* fcall = new FCallExp();
            fcall->nombre = name;

            if (!check(Token::PD)) {
                fcall->argumentos.push_back(parseCExp());
                while (match(Token::COMA)) {
                    fcall->argumentos.push_back(parseCExp());
                }
            }

            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' después de los argumentos." << endl;
                exit(1);
            }

            return fcall;
        } else {
            return new IdentifierExp(name);
        }
    }
    else if (match(Token::IFEXP)) {
        match(Token::PI);
        e=parseCExp();
        match(Token::COMA);
        e1=parseCExp();
        match(Token::COMA);
        e2=parseCExp();
        match(Token::PD);
        return new IFExp(e,e1,e2);
    }
    else if (match(Token::PI)){
        e = parseCExp();
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}