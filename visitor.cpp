#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// Accept methods para todas las clases de AST
///////////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IFExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FCallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ReturnStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Program::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecWithInit::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// PrintVisitor Implementation
///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(FloatExp* exp) {
    cout << exp->value << "f";
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";  // Agregar punto y coma
}

void PrintVisitor::visit(PrintStatement* stm) {
    if (stm->println) {
        cout << "println(";
    } else {
        cout << "print(";
    }
    stm->e->accept(this);
    cout << ");";  // Agregar punto y coma
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if ";
    stm->condition->accept(this);
    cout << " then" << endl;
    stm->then->accept(this);
    if(stm->els){
        cout << "else" << endl;
        stm->els->accept(this);
    }
    cout << "endif";
}

void PrintVisitor::visit(WhileStatement* stm) {
    cout << "while ";
    stm->condition->accept(this);
    cout << " do" << endl;
    stm->b->accept(this);
    cout << "endwhile";
}

void PrintVisitor::visit(ForStatement* stm) {
    cout << "for " << stm->var << " in ";
    stm->start->accept(this);
    cout << " to ";
    stm->end->accept(this);
    cout << " do" << endl;
    stm->body->accept(this);
    cout << "endfor";
}

int PrintVisitor::visit(IFExp* pepito) {
    cout<< "ifexp(";
    pepito->cond->accept(this);
    cout<< ",";
    pepito->left->accept(this);
    cout<< ",";
    pepito->right->accept(this);
    cout << ")" ;
    return 0;
}

void PrintVisitor::visit(VarDec* stm) {
    for(auto i = stm->vars.begin(); i != stm->vars.end(); ++i){
        cout << "var " << *i << ": " << stm->type;
        if(next(i) != stm->vars.end()) cout << ", ";
    }
    cout << ";";  // Agregar punto y coma
}

void PrintVisitor::visit(VarDecWithInit* dec) {
    cout << "var " << dec->id << ": " << dec->type << " = ";
    dec->init_value->accept(this);
    cout << ";";  // Agregar punto y coma
}

void PrintVisitor::visit(VarDecList* stm) {
    for(auto i: stm->vardecs){
        cout << "   "; // Indentación
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm) {
    for(auto i: stm->stms){
        cout << "   "; // Indentación para statements
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm) {
    cout << " {" << endl;
    if (stm->vardecs && !stm->vardecs->vardecs.empty()) {
        stm->vardecs->accept(this);
    }
    if (stm->slist && !stm->slist->stms.empty()) {
        stm->slist->accept(this);
    }
    cout << "}";
}

int PrintVisitor::visit(FCallExp* e) {
    cout << e->nombre << "(";
    for(auto i = e->argumentos.begin(); i != e->argumentos.end(); ++i) {
        (*i)->accept(this);
        if (next(i) != e->argumentos.end()) cout << ",";
    }
    cout << ")";
    return 0;
}

void PrintVisitor::visit(FunDec* e) {
    cout << "fun " << e->nombre << "(";

    auto param_it = e->parametros.begin();
    auto type_it = e->tipos.begin();

    while (param_it != e->parametros.end()) {
        cout << *param_it << ": " << *type_it;
        ++param_it;
        ++type_it;
        if (param_it != e->parametros.end()) cout << ", ";
    }
    cout << ")";

    // Imprimir tipo de retorno si no es Unit o void
    if (!e->tipo.empty() && e->tipo != "void" && e->tipo != "Unit") {
        cout << ": " << e->tipo;
    }

    if (e->cuerpo) e->cuerpo->accept(this);
    cout << endl;
}

void PrintVisitor::visit(FunDecList* e) {
    for(auto i: e->Fundecs){
        i->accept(this);
    }
}

void PrintVisitor::visit(ReturnStatement* e) {
    cout << "return";
    if (e->e != nullptr) {
        cout << " ";
        e->e->accept(this);
    }
    cout << ";";  // Agregar punto y coma
}

void PrintVisitor::visit(Program* p) {
    if (p->fundecs) {
        p->fundecs->accept(this);
    }
}

void PrintVisitor::imprimir(Program* program) {
    program->accept(this);
}

///////////////////////////////////////////////////////////////////////////////////
// EVALVisitor Implementation
///////////////////////////////////////////////////////////////////////////////////

int EVALVisitor::visit(BinaryExp* exp) {
    int result;
    int v1 = exp->left->accept(this);
    int v2 = exp->right->accept(this);
    switch(exp->op) {
        case PLUS_OP: result = v1 + v2; break;
        case MINUS_OP: result = v1 - v2; break;
        case MUL_OP: result = v1 * v2; break;
        case DIV_OP:
            if(v2 != 0) result = v1 / v2;
            else {
                cout << "Error: división por cero" << endl;
                result = 0;
            }
            break;
        case LT_OP: result = v1 < v2; break;
        case LE_OP: result = v1 <= v2; break;
        case GE_OP: result = v1 >= v2; break;
        case GT_OP: result = v1 > v2; break;
        case EQ_OP: result = v1 == v2; break;
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(FloatExp* exp) {
    return static_cast<int>(exp->value * 100);
}

int EVALVisitor::visit(BoolExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(IdentifierExp* exp) {
    if(env.check(exp->name)){
        return env.lookup(exp->name);
    }
    else{
        cout << "Variable no declarada: " << exp->name << endl;
        return 0;
    }
}

void EVALVisitor::visit(AssignStatement* stm) {
    if(!env.check(stm->id)){
        cout << "Variable no declarada: " << stm->id << endl;
        return;
    }

    int value = stm->rhs->accept(this);
    env.update(stm->id, value);
}

void EVALVisitor::visit(PrintStatement* stm) {
    // Verificar si la expresión es un identificador y obtener su tipo
    if (IdentifierExp* idExp = dynamic_cast<IdentifierExp*>(stm->e)) {
        string varType = env.getType(idExp->name);
        int value = stm->e->accept(this);

        if (varType == "Float" || varType == "TYPEFLOAT") {
            cout << (value / 100.0) << "f";
        } else {
            cout << value;
        }
    }
    // Si es directamente un FloatExp
    else if (FloatExp* floatExp = dynamic_cast<FloatExp*>(stm->e)) {
        cout << floatExp->value << "f";
    }
    // Para otros casos
    else {
        int value = stm->e->accept(this);

        // Heurística: si el valor es mayor a 1000, probablemente es un float escalado
        if (value > 1000 && value % 100 != 0) {
            cout << (value / 100.0) << "f";
        } else if (value > 1000) {
            cout << (value / 100) << "f";
        } else {
            cout << value;
        }
    }

    if (stm->println) {
        cout << endl;
    }
}

void EVALVisitor::visit(IfStatement* stm) {
    if(stm->condition->accept(this)){
        stm->then->accept(this);
    }
    else{
        if(stm->els) stm->els->accept(this);
    }
}

void EVALVisitor::visit(WhileStatement* stm) {
    while(stm->condition->accept(this)){
        stm->b->accept(this);
        if (retcall) break;
    }
}

void EVALVisitor::visit(ForStatement* stm) {
    int start_val = stm->start->accept(this);
    int end_val = stm->end->accept(this);

    env.add_level();
    env.add_var(stm->var, "int");

    for(int i = start_val; i <= end_val; i++) {
        env.update(stm->var, i);
        stm->body->accept(this);
        if (retcall) break;
    }

    env.remove_level();
}

int EVALVisitor::visit(IFExp* pepito) {
    if(pepito->cond->accept(this)){
        return pepito->left->accept(this);
    }
    else{
        return pepito->right->accept(this);
    }
}

void EVALVisitor::visit(VarDec* stm) {
    list<string>::iterator it;
    for(it = stm->vars.begin(); it != stm->vars.end(); it++){
        env.add_var(*it, stm->type);
    }
}

void EVALVisitor::visit(VarDecWithInit* stm) {
    int value = stm->init_value->accept(this);

    // Agregar la variable al entorno con su tipo
    env.add_var(stm->id, stm->type);
    env.update(stm->id, value);
}

void EVALVisitor::visit(VarDecList* stm) {
    list<VarDec*>::iterator it;
    for(it = stm->vardecs.begin(); it != stm->vardecs.end(); it++){
        (*it)->accept(this);
    }
}

void EVALVisitor::visit(StatementList* stm) {
    list<Stm*>::iterator it;
    for (it = stm->stms.begin(); it != stm->stms.end(); it++) {
        (*it)->accept(this);
        if (retcall) break;
    }
}

void EVALVisitor::visit(Body* b) {
    env.add_level();
    if (b->vardecs) b->vardecs->accept(this);
    if (b->slist) b->slist->accept(this);
    env.remove_level();
}

int EVALVisitor::visit(FCallExp* e) {
    if (fdecs.find(e->nombre) == fdecs.end()) {
        cout << "Error: Función no declarada: " << e->nombre << endl;
        return 0;
    }

    FunDec* func = fdecs[e->nombre];

    if (e->argumentos.size() != func->parametros.size()) {
        cout << "Error: Número incorrecto de argumentos para función " << e->nombre << endl;
        return 0;
    }

    env.add_level();

    auto arg_it = e->argumentos.begin();
    auto param_it = func->parametros.begin();
    auto type_it = func->tipos.begin();

    while (arg_it != e->argumentos.end()) {
        int value = (*arg_it)->accept(this);
        env.add_var(*param_it, *type_it);
        env.update(*param_it, value);
        ++arg_it;
        ++param_it;
        ++type_it;
    }

    bool prev_retcall = retcall;
    int prev_retval = retval;
    retcall = false;

    func->cuerpo->accept(this);

    int result = retval;
    retcall = prev_retcall;
    retval = prev_retval;

    env.remove_level();

    return result;
}

void EVALVisitor::visit(FunDec* e) {
    fdecs[e->nombre] = e;
}

void EVALVisitor::visit(FunDecList* e) {
    for(auto func : e->Fundecs) {
        func->accept(this);
    }
}

void EVALVisitor::visit(ReturnStatement* e) {
    if (e->e != nullptr) {
        retval = e->e->accept(this);
    } else {
        retval = 0;
    }
    retcall = true;
}

void EVALVisitor::visit(Program* p) {
    env.add_level();

    // Procesar declaraciones de variables globales si existen
    if (p->vardecs) {
        p->vardecs->accept(this);
    }

    // Procesar declaraciones de funciones
    if (p->fundecs) {
        p->fundecs->accept(this);
    }

    // Buscar y ejecutar main
    if (fdecs.find("main") == fdecs.end()) {
        cout << "Error: No se encontró la función main" << endl;
        exit(0);
    }

    FunDec* main_dec = fdecs["main"];
    retcall = false;

    // Ejecutar el cuerpo de main
    if (main_dec->cuerpo) {
        main_dec->cuerpo->accept(this);
    }

    env.remove_level();
}

void EVALVisitor::ejecutar(Program* program) {
    program->accept(this);
}