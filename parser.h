#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "scanner.h"
#include "exp.h"
#include "token.h"

class Parser {
private:
    vector<Token> tokens;
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
    Parser(const vector<Token>& tokens) : tokens(tokens) {}
};

#endif // PARSER_H