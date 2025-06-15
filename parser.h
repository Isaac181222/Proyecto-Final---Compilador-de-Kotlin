#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "exp.h"

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    list<Stm*> parseStmList();
    Exp* parseCExp();
    Exp* parseExpression();
    Exp* parseTerm();
    Exp* parseFactor();
    void expect(Token::Type type);
public:
    Parser(Scanner* scanner);
    Program* parseProgram();
    Stm* parseStatement();
    StatementList* parseStatementList();
    Stm* parseVarDec();
    VarDecList* parseVarDecList();
    Body* parseBody();
    FunDecList* parseFunDecList();
    FunDec* parseFunDec();
    Stm* parseVarDecWithInit();
};

#endif // PARSER_H