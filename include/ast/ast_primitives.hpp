#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "symbol_table.hpp"

extern SymbolTable Symbol;
extern StackPtr StackPointer;

enum TypeDef{
  INT,
  FLT,
  DBL
};

enum DeclType{
  CALL,
  ASSIGN,
  DECL,
  ARG
};

class Variable
    :public Expression
{
private:
    std::string type;
    std::string id;
    mutable std::string address;
    std::string assignop;
    DeclType VarType;
    ExpressionPtr Expr = nullptr;
public:
//This constructor does not feel right, it's a placeholder.
    Variable() {
    }
    
    Variable(const std::string *_id) {
        VarType = CALL;
        id = *_id;
    }

    Variable(const std::string *_id, std::string *_AssignOp, ExpressionPtr _Expr) {
        VarType = ASSIGN;
        id = *_id;
        assignop = *_AssignOp;
        Expr = _Expr;
    }

    Variable(TypeDef _type, const std::string *_id, DeclType _form, ExpressionPtr _Expr = nullptr) {
        if(_form == ARG){
            VarType = ARG;
            switch(_type) {
                case INT:
                    type = "INT";
                    id = *_id;
                    break;
                case FLT:
                    type = "FLT";
                    id = *_id;
                    /*
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    address = std::to_string(StackPointer.getIncr() + 2000);
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    */
                    break;
                case DBL:
                    type = "DBL";
                    id = *_id;
                    /*
                    StackPointer.setIncr(StackPointer.getIncr()+16);
                    address = std::to_string(StackPointer.getIncr() + 2000);
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    */
                    break;
                default:
                    type = "something went wrong";
            }
        }else if(_form == DECL){
            VarType = DECL;
            switch(_type) {
                case INT:
                    type = "INT";
                    id = *_id;
                    Expr = _Expr;
                    break;
                case FLT:
                    type = "FLT";
                    id = *_id;
                    Expr = _Expr;
                    /*
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    address = std::to_string(StackPointer.getIncr() + 2000);
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    */
                    break;
                case DBL:
                    type = "DBL";
                    id = *_id;
                    Expr = _Expr;
                    /*
                    StackPointer.setIncr(StackPointer.getIncr()+16);
                    address = std::to_string(StackPointer.getIncr() + 2000);
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    */
                    break;
                default:
                    type = "something went wrong";
            }
        }
    }
    
    const std::string getType() const
    { return type; }

    const std::string getId() const
    { return id; }

    const std::string getAddr() const
    { return address; }

    ExpressionPtr getExpr() const
    { return Expr; }

    virtual void print(std::ostream &dst) const override
    {
        switch(VarType) {
            case CALL:
                dst<<id;
                break;
            case ASSIGN:
                dst<<id;
                dst<<" ";
                dst<<assignop;
                dst<<" ";
                Expr->print(dst);
                break;
            case DECL:
                dst<<type;
                dst<<" ";
                dst<<id;
                if(Expr!=nullptr){
                    dst<<" = ";
                    Expr->print(dst);
                }
                dst<<";";
                dst<<'\n';
                break;
        }
    }

    virtual void CompileRec(std::string destReg) const override{
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                std::cout << "lw " << destReg << ", -" << address << "($fp)" << std::endl;
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                if(assignop == "="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                }else if(assignop == "+="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "add $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "-="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "sub $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "/="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "div $t1, $t0" << std::endl;
                    std::cout << "mfhi $t1" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "*="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "mul $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "%="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "div $t1, $t0" << std::endl;
                    std::cout << "mflo $t1" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "<<="){

                }else if(assignop == ">>="){

                }else if(assignop == "^="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "xor $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "&="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "and $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }else if(assignop == "|="){
                    getExpr()->CompileRec("$t0");
                    std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                    std::cout << "or $t1, $t1, $t0" << std::endl;
                    std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                }
                break;
            case DECL:
                if(getType()=="INT"){
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Expr!=nullptr){
                        getExpr()->CompileRec("$t0");
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0){
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                break;
            case ARG:
                if(getType()=="INT"){
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    if(StackPointer.getArgc()<4){
                        std::cout << "sw $a" << StackPointer.getArgc() << ", 0($sp)"<<  std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Expr!=nullptr){
                        getExpr()->CompileRec("$t0");
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0){
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                break;
        }   
    }
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return bindings.at(id);
    }    
};

class DeclarationList;

typedef const DeclarationList *DeclarationListPtr;

class DeclarationList
    : public Variable
{
private:
    Variable *variable;
    DeclarationListPtr declarationList = nullptr;
public:
    DeclarationList(Variable *_variable, DeclarationListPtr _declarationList = nullptr)
        : variable(_variable)
        , declarationList(_declarationList)
    {}

    virtual ~DeclarationList() {
        delete variable;
        delete declarationList;
    }
    Variable *getVar() const
    { return variable; }

    DeclarationListPtr getdecllist() const
    { return declarationList; }

    virtual void print(std::ostream &dst) const override
    {
        variable->print(dst);
        if(declarationList!=nullptr){
            dst << ", ";
            declarationList->print(dst);
        }
    }

    virtual void CompileRec(std::string destReg) const override{
        getVar()->CompileRec(destReg);
        StackPointer.setArgc(StackPointer.getArgc()+1);
        if(declarationList!=nullptr){
            getdecllist()->CompileRec(destReg);
        }
        StackPointer.setArgc(0);
    }  
};

class Number
    : public Expression
{
private:
    double value;
public:
    Number(double _value)
        : value(_value)
    {}

    double getValue() const
    { return value; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<value;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return value;
    }

    virtual void CompileRec(std::string destReg) const override{
        std::cout << "addi " << destReg << ", $0, " << value << std::endl;
    }  

};

#endif
