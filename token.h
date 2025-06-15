#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        PLUS,FUN,ENDFUN,RETURN, MINUS, MUL, DIV, NUM, FLOATNUM, ERR, PD, PI,
        END, ID, PRINT, PRINTLN, ASSIGN, PC,LT, LE, EQ, IF, THEN, ELSE, ENDIF,
        WHILE,DO,ENDWHILE,COMA,IFEXP, VAR, FOR, ENDFOR, TRUE, FALSE,
        TYPEINT, TYPEFLOAT, COLON, LLAVEI, LLAVED, GE, GT
    };

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H