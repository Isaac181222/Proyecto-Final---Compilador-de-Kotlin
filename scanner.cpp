#include <iostream>
#include <cstring>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length() &&  is_white_space(input[current]) ) current++;
    if (current >= input.length()) return new Token(Token::END);
    char c  = input[current];
    first = current;
    if (isdigit(c)) {
        bool isFloat = false;
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;

        if (current < input.length() && input[current] == '.') {
            isFloat = true;
            current++;
            while (current < input.length() && isdigit(input[current]))
                current++;
            if (current < input.length() && (input[current] == 'f' || input[current] == 'F')) {
                current++;
            }
        }

        if (isFloat)
            token = new Token(Token::FLOATNUM, input, first, current - first);
        else
            token = new Token(Token::NUM, input, first, current - first);
    }

    else if (isalpha(c)) {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);
        if (word == "print") {
            token = new Token(Token::PRINT, input, first, current - first);
        } else if (word == "if") {
            token = new Token(Token::IF, input, first, current - first);
        } else if (word == "then") {
            token = new Token(Token::THEN, input, first, current - first);
        } else if (word == "else") {
            token = new Token(Token::ELSE, input, first, current - first);
        } else if (word == "endif") {
            token = new Token(Token::ENDIF, input, first, current - first);
        } else if (word == "ifexp") {
            token = new Token(Token::IFEXP, input, first, current - first);
        } else if (word == "while") {
            token = new Token(Token::WHILE, input, first, current - first);
        } else if (word == "endwhile") {
            token = new Token(Token::ENDWHILE, input, first, current - first);
        }
        else if (word == "do") {
            token = new Token(Token::DO, input, first, current - first);
        }else if (word == "for") {
            token = new Token(Token::FOR, input, first, current - first);
        }
        else if (word == "endfor") {
            token = new Token(Token::ENDFOR, input, first, current - first);
        }
        else if (word == "var") {
            token = new Token(Token::VAR, input, first, current - first);
        }
        else if (word == "true") {
            token = new Token(Token::TRUE, input, first, current - first);
        }
        else if (word == "false") {
            token = new Token(Token::FALSE, input, first, current - first);
        }
        else if (word == "fun") {
            token = new Token(Token::FUN, input, first, current - first);
        }
        else if (word == "endfun") {
            token = new Token(Token::ENDFUN, input, first, current - first);
        }
        else if (word == "return") {
            token = new Token(Token::RETURN, input, first, current - first);
        }
        else if (word == "Int") {
            token = new Token(Token::TYPEINT, input, first, current - first);
        }
        else if (word == "Float") {
            token = new Token(Token::TYPEFLOAT, input, first, current - first);
        }
        else if (word == "println") {
            token = new Token(Token::PRINTLN, input, first, current - first);
        }
        else {
            token = new Token(Token::ID, input, first, current - first);
        }
    }

    else if (strchr("+-*/()=;,<>{}:", c)) {
        switch(c) {
            case '+': token = new Token(Token::PLUS, input, current, 1); break;
            case '-': token = new Token(Token::MINUS, input, current, 1); break;
            case '*': token = new Token(Token::MUL, input, current, 1); break;
            case '/': token = new Token(Token::DIV, input, current, 1); break;
            case ',': token = new Token(Token::COMA, input, current, 1); break;
            case '(': token = new Token(Token::PI, input, current, 1); break;
            case ')': token = new Token(Token::PD, input, current, 1); break;
            case ':': token = new Token(Token::COLON, input, current, 1); break;
            case '{': token = new Token(Token::LLAVEI, input, current, 1); break;
            case '}': token = new Token(Token::LLAVED, input, current, 1); break;
            case '=':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::EQ, input, current, 2);
                    current++;
                } else {
                    token = new Token(Token::ASSIGN, input, current, 1);
                }
                break;
            case '<':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::LE, input, current, 2);
                    current++;
                } else {
                    token = new Token(Token::LT, input, current, 1);
                }
                break;
            case '>':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::GE, input, current, 2);
                    current++;
                } else {
                    token = new Token(Token::GT, input, current, 1);
                }
                break;
            case ';': token = new Token(Token::PC, input, current, 1); break;
            default:
                cout << "No debería llegar acá" << endl;
                token = new Token(Token::ERR, input, current, 1);
        }
        current++;
    }
    else {
        token = new Token(Token::ERR, input, current, 1);
        current++;
    }
    return token;
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() { }

void test_scanner(Scanner* scanner) {
    Token* current;
    cout << "Iniciando Scanner:" << endl<< endl;
    while ((current = scanner->nextToken())->type != Token::END) {
        if (current->type == Token::ERR) {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}