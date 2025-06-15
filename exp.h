#ifndef EXP_H
#define EXP_H

#include <string>
#include <unordered_map>
#include <list>
using namespace std;

class Visitor;

enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, EQ_OP, GT_OP, GE_OP };

class Body;

class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};
class IFExp : public Exp {
public:
    Exp *cond,*left, *right;
    IFExp(Exp *cond, Exp* l, Exp* r);
    int accept(Visitor* visitor);
    ~IFExp();
};


class BinaryExp : public Exp {
public:
    Exp *left, *right;
    string type;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    ~BinaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    ~IdentifierExp();
};

class FloatExp : public Exp {
public:
    float value;
    FloatExp(float v);
    int accept(Visitor* visitor);
    ~FloatExp();
};


class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    int accept(Visitor* visitor);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    Exp* e;
    bool println; // Para distinguir entre print y println
    PrintStatement(Exp* e, bool println = false);
    int accept(Visitor* visitor);
    ~PrintStatement();
};


class IfStatement : public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStatement(Exp* condition, Body* then, Body* els);
    int accept(Visitor* visitor);
    ~IfStatement();
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStatement();
};



class VarDecWithInit : public Stm {
public:
    std::string id;
    std::string type;
    Exp* init_value;

    VarDecWithInit(std::string id, std::string type, Exp* init);
    int accept(Visitor* visitor);
    ~VarDecWithInit();
};

// VarDec ahora hereda de Stm para poder ser usado en StatementList
class VarDec : public Stm {
public:
    string type;
    list<string> vars;
    VarDec(string type, list<string> vars);
    int accept(Visitor* visitor);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    ~StatementList();
};


class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    int accept(Visitor* visitor);
    Body(VarDecList* vardecs, StatementList* stms);
    ~Body();
};

class ForStatement : public Stm {
public:
    std::string var;   // nombre de la variable
    Exp* start;
    Exp* end;
    Body* body;

    ForStatement(std::string var, Exp* start, Exp* end, Body* body)
        : var(var), start(start), end(end), body(body) {}

    int accept(Visitor* visitor) override;
    ~ForStatement() {
        delete start;
        delete end;
        delete body;
    }
};

class FunDec {
public:
    string nombre;
    string tipo;
    list<string> parametros;
    list<string> tipos;
    Body* cuerpo;
    FunDec(){};
    ~FunDec(){delete cuerpo;};
    int accept(Visitor* visitor);
};

class FCallExp : public Exp {
public:
    string nombre;
    list<Exp*> argumentos;
    FCallExp(){};
    ~FCallExp(){
        for(auto arg : argumentos) {
            delete arg;
        }
    };
    int accept(Visitor* visitor);
};

class FunDecList{
public:
    list<FunDec*> Fundecs;
    void add(FunDec* fundec) {
        Fundecs.push_back(fundec);
    };
    int accept(Visitor* visitor);
    FunDecList(){};
    ~FunDecList(){
        for(auto f : Fundecs) {
            delete f;
        }
    };
};

class ReturnStatement: public Stm {
public:
    Exp* e;
    ReturnStatement(Exp* exp = nullptr): e(exp) {};
    ~ReturnStatement(){if(e) delete e;};
    int accept(Visitor* visitor);
};

class Program {
public:
    VarDecList* vardecs;
    FunDecList* fundecs;
    Program(VarDecList* v, FunDecList* f) : vardecs(v), fundecs(f) {}
    ~Program() {
        delete vardecs;
        delete fundecs;
    }
    int accept(Visitor* visitor);
};

class FCallStatement : public Stm {
public:
    FCallExp* fcall;
    FCallStatement(FCallExp* fcall) : fcall(fcall) {}
    int accept(Visitor* visitor) override;
    ~FCallStatement() {
        delete fcall;
    }
};

#endif // EXP_H