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
    if (!match(Token::VAR)) return nullptr;

    if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador después de 'var'." << endl;
        exit(1);
    }
    string varname = previous->text;
    list<string> ids = { varname };

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
        init = parseExpression();  // asegúrate que parseExpression esté bien implementado
    }

    if (!match(Token::PC)) {
        cout << "Error: se esperaba ';' después de la declaración de variable." << endl;
        exit(1);
    }

    if (init != nullptr) {
        return new VarDecWithInit(type, varname, init);  // asegúrate de tener esta clase definida
    }

    return new VarDec(type, ids);
}


VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    while (true) {
        Token* saved = current;

        Stm* aux = parseVarDec();
        if (aux == nullptr) {
            // Restauramos el token si parseVarDec no era una declaración válida sin asignación
            current = saved;
            break;
        }

        if (auto* vd = dynamic_cast<VarDec*>(aux)) {
            vdl->add(vd);
        }
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    while (!check(Token::LLAVED) && !isAtEnd()) {
        Stm* stmt = parseStatement();
        if (stmt != nullptr) {
            cout << "[DEBUG] Añadido a StatementList: " << typeid(*stmt).name() << endl;
            sl->add(stmt);
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
    // En Kotlin, típicamente no hay variables globales antes de las funciones
    // Así que creamos una lista vacía de variables globales
    VarDecList* vardecs = new VarDecList();

    // Parseamos las funciones directamente
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

FunDec* Parser::parseFunDec() {
    FunDec* fd = nullptr;

    if (match(Token::FUN)) {
        fd = new FunDec();

        if (!match(Token::ID)) {
            cout << "Error: se esperaba nombre de función después de 'fun', pero se encontró '" << current->text << "'" << endl;
            exit(1);
        }
        fd->nombre = previous->text;
        fd->tipo = "void"; // valor por defecto

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

            if (!match(Token::ID)) {
                cout << "Error: se esperaba tipo del parámetro después de ':', pero se encontró '" << current->text << "'" << endl;
                exit(1);
            }
            string paramType = previous->text;

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
    Token* saved = current;
    Stm* dec = parseVarDecWithInit();
    if (dec != nullptr) return dec;
    Stm* s = NULL;
    Exp* e = NULL;
    Body* tb = NULL;
    Body* fb = NULL;

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (match(Token::VAR)) {
        if (!match(Token::ID)) {
            cout << "Error: se esperaba identificador después de 'var'." << endl;
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

        if (match(Token::ASSIGN)) {
            Exp* expr = parseCExp();
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' después de la asignación." << endl;
                exit(1);
            }
            return new VarDecWithInit(varname, type, expr);
        } else {
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' después de la declaración." << endl;
                exit(1);
            }
            list<string> vars = { varname };
            return new VarDec(type, vars);
        }
    }
    else if (match(Token::ID)) {
        string lex = previous->text;

        if (match(Token::ASSIGN)) {
            e = parseCExp();
            s = new AssignStatement(lex, e);

            // Expect semicolon
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' después de la asignación." << endl;
                exit(1);
            }
        }
        else if (match(Token::PI)) {
            // This is a function call - create as FCallExp and wrap in PrintStatement
            // Based on your exp.h, there's no FCallStatement, so we use PrintStatement for function calls
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

            // Use PrintStatement to handle function calls (as in original code)
            s = new PrintStatement(fcall);

            // Expect semicolon
            if (!match(Token::PC)) {
                cout << "Error: se esperaba ';' después de la llamada a función." << endl;
                exit(1);
            }
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

        // Expect semicolon
        if (!match(Token::PC)) {
            cout << "Error: se esperaba ';' después de print." << endl;
            exit(1);
        }
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

        // Expect semicolon
        if (!match(Token::PC)) {
            cout << "Error: se esperaba ';' después de println." << endl;
            exit(1);
        }
    }
    else if (match(Token::RETURN)) {
        ReturnStatement* rs = new ReturnStatement();
        if (match(Token::PI)) {
            if (!check(Token::PD)){
                rs->e = parseCExp();
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' después de return." << endl;
                exit(1);
            }
        }
        s = rs;

        // Expect semicolon
        if (!match(Token::PC)) {
            cout << "Error: se esperaba ';' después de return." << endl;
            exit(1);
        }
    }
    else if (match(Token::IF)) {
        e = parseCExp();
        if (!match(Token::THEN)) {
            cout << "Error: se esperaba 'then' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (match(Token::ELSE)) {
            fb = parseBody();
        }
        if (!match(Token::ENDIF)) {
            cout << "Error: se esperaba 'end' al final de la declaración." << endl;
            exit(1);
        }
        s = new IfStatement(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        e = parseCExp();
        if (!match(Token::DO)) {
            cout << "Error: se esperaba 'do' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::ENDWHILE)) {
            cout << "Error: se esperaba 'endwhile' al final de la declaración." << endl;
            exit(1);
        }
        s = new WhileStatement(e, tb);
    }
    else {
        return NULL;
    }

    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ)){
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

Stm* Parser::parseVarDecWithInit() {
    if (!match(Token::VAR)) return nullptr;

    if (!match(Token::ID)) {
        cout << "Error: se esperaba identificador después de 'var'" << endl;
        exit(1);
    }
    string name = previous->text;

    if (!match(Token::COLON)) {
        cout << "Error: se esperaba ':' después del identificador" << endl;
        exit(1);
    }

    string type;
    if (match(Token::TYPEINT)) {
        type = "Int";
    } else if (match(Token::TYPEFLOAT)) {
        type = "Float";
    } else {
        cout << "Error: se esperaba tipo válido después de ':'" << endl;
        exit(1);
    }

    if (!match(Token::ASSIGN)) {
        cout << "[DEBUG] No se encontró '=' después de tipo. No es VarDecWithInit." << endl;
        return nullptr;
    }

    Exp* init = parseExpression();

    expect(Token::PC);  // punto y coma obligatorio

    cout << "[DEBUG] Se reconoció un VarDecWithInit para: " << name << " = ... de tipo " << type << endl;
    return new VarDecWithInit(name, type, init);
}

