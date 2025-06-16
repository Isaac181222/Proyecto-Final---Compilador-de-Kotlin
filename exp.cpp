#include <iostream>
#include "exp.h"
#include "visitor.h"
using namespace std;
IFExp::IFExp(Exp* cond,Exp* l, Exp* r): cond(cond),left(l),right(r){}
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {
    if (op == PLUS_OP || op == MINUS_OP || op == MUL_OP || op == DIV_OP) {
        type = "int";
    } else {
        type = "bool";
    }
}
NumberExp::NumberExp(int v):value(v) {}
BoolExp::BoolExp(bool v):value(v) {}
IdentifierExp::IdentifierExp(const string& n):name(n) {}
Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
IFExp::~IFExp() {delete cond, delete left; delete right; }
NumberExp::~NumberExp() { }
BoolExp::~BoolExp() { }
IdentifierExp::~IdentifierExp() { }
AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}
PrintStatement::PrintStatement(Exp* e, bool println): e(e), println(println) {}
PrintStatement::~PrintStatement() {
    delete e;
}

VarDecWithInit::VarDecWithInit(std::string id, std::string type, Exp* init)
    : id(id), type(type), init_value(init) {}
VarDecWithInit::~VarDecWithInit() {
    delete init_value;
}

IfStatement::IfStatement(Exp* c, Body* t, Body* e): condition(c), then(t), els(e) {}
IfStatement::~IfStatement() {
    delete condition;
    delete then;
    delete els;
}
WhileStatement::WhileStatement(Exp* c, Body* t): condition(c), b(t) {}
WhileStatement::~WhileStatement() {
    delete condition;
    delete b;
}

FloatExp::FloatExp(float v): value(v) {}
FloatExp::~FloatExp() {}
int FloatExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

VarDec::VarDec(string type, list<string> ids): type(type), vars(ids) {}
VarDec::~VarDec() {}

VarDecList::VarDecList(): vardecs() {}
void VarDecList::add(VarDec* v) {
    vardecs.push_back(v);
}
VarDecList::~VarDecList() {
    for (auto v: vardecs) {
        delete v;
    }
}

StatementList::StatementList(): stms() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}
StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

Body::Body(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Body::~Body() {
    delete vardecs;
    delete slist;
}


Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
    string  c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case EQ_OP: c = "=="; break;
        default: c = "$";
    }
    return c;
}