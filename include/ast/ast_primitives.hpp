#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

#include "symbol_table.hpp"

enum TypeDef{
  INT,
  FLOAT,
  DOUBLE
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
    mutable std::string type;
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
                case FLOAT:
                    type = "FLOAT";
                    id = *_id;
                    break;
                case DOUBLE:
                    type = "DOUBLE";
                    id = *_id;
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
                case FLOAT:
                    type = "FLOAT";
                    id = *_id;
                    Expr = _Expr;
                    break;
                case DOUBLE:
                    type = "DOUBLE";
                    id = *_id;
                    Expr = _Expr;
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

    virtual const std::string getDataType() const override {
        type = Symbol.getType(id);
        return type;
    }

    virtual void CompileRec(std::string destReg) const override{
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(type == "INT") {
                    std::cout << "lw " << destReg << ", -" << address << "($fp)" << std::endl;
                }
                else if(type == "FLOAT") {
                    std::cout << "lwc1 " << destReg << ", -" << address << "($fp)" << std::endl;
                }
                else if(type == "DOUBLE") {
                    std::cout << "lwc1 " << destReg << ", -" << address << "($fp)" << std::endl;
                    std::cout << "lwc1 " << destReg[0] << destReg[1] << (int)destReg[2]-48+1 << ", -" << std::stoi(address)-4 << "($fp)" << std::endl;
                }
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(assignop == "="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;  
                    }
                    else if(type == "DOUBLE") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 $f1, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                    else {
                        std::cout << getType() << std::endl;
                        std::cout << "ERROR: type missing" << std::endl;
                    }
                }else if(assignop == "+="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "add $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "add.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                        std::cout << "add.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 #f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                }else if(assignop == "-="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "sub $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "sub.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                        std::cout << "sub.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 #f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                }else if(assignop == "/="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mflo $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2 -" << address << "($fp)" << std::endl;
                        std::cout << "div.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                        std::cout << "div.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 #f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                }else if(assignop == "*="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "mul $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2 -" << address << "($fp)" << std::endl;
                        std::cout << "mul.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getExpr()->CompileRec("$f0");
                        std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                        std::cout << "mul.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 #f3, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                }else if(assignop == "%="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mfhi $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                }else if(assignop == "<<="){

                }else if(assignop == ">>="){

                }else if(assignop == "^="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "xor $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                }else if(assignop == "&="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "and $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                }else if(assignop == "|="){
                    if(type == "INT") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "or $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                }else if(assignop == "++") {
                    if(type == "INT") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        std::cout << "addi $t0, $t0, 1" << std::endl;
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "addi.s $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                        std::cout << "addi.d $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                    }
                }else if(assignop == "--") {
                    if(type == "INT") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        std::cout << "subi $t0, $t0, 1" << std::endl;
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "subi.s $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "lwc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                        std::cout << "subi.d $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                    }
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
                else if(getType() == "FLOAT") {
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference") {
                        Symbol.insert(type, "var", id, address);
                    }else {
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Expr!=nullptr) {
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0) {
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                else if(getType() == "DOUBLE") {
                    std::cout << "addi $sp, $sp, -8" << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference") {
                        Symbol.insert(type, "var", id, address);
                    }else {
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Expr!=nullptr) {
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 $f1, -" << std::stoi(address)-4 << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0) {
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
                else if(getType() == "FLOAT") {
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    if(StackPointer.getArgc()<4){
                        if(StackPointer.getArgc() == 0) {
                            std::cout << "swc1 $f12, 0($sp)"<<  std::endl;
                        }
                        else if(StackPointer.getArgc() == 1) {
                            std::cout << "swc1 $f14, 0($sp)"<<  std::endl;
                        }
                        else if(StackPointer.getArgc() == 2) {
                            std::cout << "sw $6, 0($sp)"<<  std::endl;
                        }
                        else if(StackPointer.getArgc() == 3) {
                            std::cout << "sw $7, 0($sp)"<<  std::endl;
                        }
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
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0){
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                else if(getType() == "DOUBLE") {
                    std::cout << "addi $sp, $sp, -8" << std::endl;
                    if(StackPointer.getArgc()<4){
                        int float_argNum;
                        if(StackPointer.getArgc() == 0) {
                            float_argNum = 12;
                        }
                        else if(StackPointer.getArgc() == 1) {
                            float_argNum = 14;
                        }
                        else if(StackPointer.getArgc() == 2) {
                            float_argNum = 16;
                        }
                        else if(StackPointer.getArgc() == 3) {
                            float_argNum = 18;
                        }
                        std::cout << "swc1 $f" << float_argNum << ", 0($sp)"<<  std::endl;
                        std::cout << "swc1 $f" << float_argNum+1 << ", 4($sp)" << std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Expr!=nullptr){
                        getExpr()->CompileRec("$f0");
                        std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        std::cout << "swc1 $f1, -" << std::stoi(address)-4 << "($fp)" << std::endl;
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

class Array
    : public Variable
{
private:
    mutable std::string type;
    std::string id;
    mutable std::string address;
    double length;
    ExpressionPtr index;
    std::string assignop;
    DeclType VarType;
    ExpressionPtr Expr = nullptr;
public:
//This constructor does not feel right, it's a placeholder.
    Array() {}

    Array(const std::string *_id, ExpressionPtr _index) {
        VarType = CALL;
        id = *_id;
        index = _index;
    }

    Array(const std::string *_id, ExpressionPtr _index, std::string *_assignop, ExpressionPtr _Expr) {
        VarType = ASSIGN;
        id = *_id;
        assignop = *_assignop;
        Expr = _Expr;
        index = _index;
    }

    Array(TypeDef _type, const std::string *_id, double _length) {
        VarType = DECL;
        id = *_id;
        length = _length;
        switch(_type) {
            case INT:
                type = "INT";
                break;
            case FLOAT:
                type = "FLOAT";
                break;
            case DOUBLE:
                type = "DOUBLE";
                break;
            default:
                type = "something went wrong";
        }
    }

    Array(TypeDef _type, const std::string *_id) {
        VarType = ARG;
        id = *_id;
        switch(_type) {
            case INT:
                type = "INT";
                break;
            case FLOAT:
                type = "FLOAT";
                break;
            case DOUBLE:
                type = "DOUBLE";
                break;
            default:
                type = "something went wrong";
        }
    }

    const std::string getType() const
    { return type; }

    const std::string getId() const
    { return id; }

    const std::string getAddr() const { // returns address of the first element
        return address;
    }

    ExpressionPtr getExpr() const { // returns expr for given index
        return Expr;
    }

    ExpressionPtr getIndex() const { // returns given index
        return index;
    }

    double getLength() const { // returns length of the array
        return length;
    }

    virtual const std::string getDataType() const override {
        type = Symbol.getType(id);
        return type;
    }

    virtual void CompileRec(std::string destReg) const override {
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                getIndex()->CompileRec("$t0");
                if(type == "INT") {
                    std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                    std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                    std::cout << "subu $t1, $t1, $t0" << std::endl;
                    std::cout << "lw " << destReg << ", 0($t1)" << std::endl;
                }
                else if(type == "FLOAT") {
                    std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                    std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                    std::cout << "subu $t1, $t1, $t0" << std::endl;
                    std::cout << "lwc1 " << destReg << ", 0($t1)" << std::endl;
                }
                else if(type == "DOUBLE") {
                    std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                    std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                    std::cout << "subu $t1, $t1, $t0" << std::endl;
                    std::cout << "lwc1 " << destReg << ", 0($t1)" << std::endl;
                    std::cout << "lwc1 " << destReg[0] << destReg[1] << (int)destReg[2]-48+1 << ", -4($t1)" << std::endl;
                }
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(assignop == "=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "subu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "sw $t0, 0($t1)" << std::endl;
                    }
                    if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "subu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                    }
                    if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addiu $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "subu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                        std::cout << "swc1 $f1, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "+=") {

                }
                else if(assignop == "-=") {

                }
                else if(assignop == "/=") {

                }
                else if(assignop == "*=") {

                }
                else if(assignop == "%=") {

                }
                else if(assignop == "<<=") {

                }
                else if(assignop == ">>=") {

                }
                else if(assignop == "^=") {

                }
                else if(assignop == "&=") {

                }
                else if(assignop == "|=") {

                }
                break;
            case DECL:
                if(getType()== "INT" || getType() == "FLOAT") {
                    std::cout << "addi $sp, $sp, -" << 4*getLength() << std::endl;
                    address = std::to_string(StackPointer.getIncr()+4);
                    StackPointer.setIncr(StackPointer.getIncr()+4*getLength());
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4*getLength());
                    if(Symbol.lookUp(id) == "Error: undefined reference") {
                        Symbol.insert(type, "array", id, address);
                    }else {
                        Symbol.modify(type, "array", id, address);
                    }
                    if(Symbol.getScope() == 0) {
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                else if(getType() == "DOUBLE") {
                    std::cout << "addi $sp, $sp, -" << 8*getLength() << std::endl;
                    address = std::to_string(StackPointer.getIncr()+4);
                    StackPointer.setIncr(StackPointer.getIncr()+8*getLength());
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8*getLength());
                    if(Symbol.lookUp(id) == "ERROR: undefined reference") {
                        Symbol.insert(type, "array", id, address);
                    }else {
                        Symbol.modify(type, "array", id, address);
                    }
                    if(Symbol.getScope() == 0) {
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                break;
        }
    }
};

class FunctionStorage
    :public Expression
{
private:
    std::string type;
    std::string id;
    mutable std::string address;
    ExpressionListPtr Args = nullptr;
    DeclType VarType;
public:
//This constructor does not feel right, it's a placeholder.
    FunctionStorage() {
    }
    
    FunctionStorage(const std::string *_id, ExpressionListPtr _Args = nullptr) {
        VarType = CALL;
        id = *_id;
        Args = _Args;
    }

    FunctionStorage(TypeDef _type, const std::string *_id) {
        VarType = DECL;
        switch(_type) {
            case INT:
                type = "INT";
                id = *_id;
                break;
            case FLOAT:
                type = "FLOAT";
                id = *_id;
                break;
            case DOUBLE:
                type = "DOUBLE";
                id = *_id;
                break;
            default:
                type = "something went wrong";
        }
    }
    
    const std::string getType() const
    { return type; }

    const std::string getId() const
    { return id; }

    const std::string getAddr() const
    { return address; }

    virtual void print(std::ostream &dst) const override
    {
        switch(VarType) {
            case CALL:
                dst<<id;
                break;
            case DECL:
                dst<<type;
                dst<<" ";
                dst<<id;
                dst<<"();";
                dst<<'\n';
                break;
        }
    }

    virtual void CompileRec(std::string destReg) const override{
        switch(VarType) {
            case CALL:
                if(Symbol.lookUp(id)[0]=='n'){
                    address = std::to_string(std::stoi(Symbol.lookUp(id).substr(1, Symbol.lookUp(id).length()-1))+44);
                    StackPointer.setNullfunc(1);
                    if(Args!=nullptr){
                        Args->CompileRec(destReg, address);
                    }
                    std::cout << "jal " << id << std::endl;
                }else{
                    address = std::to_string(std::stoi(Symbol.lookUp(id))+44);
                    if(Args!=nullptr){
                        Args->CompileRec(destReg, address);
                    }
                std::cout << "jal " << id << std::endl;
                }
                break;
            case DECL:
                if(destReg=="$0"){
                    address = "n" + std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "func", id, address);
                    }else{
                        Symbol.modify(type, "func", id, address);
                    }
                }else if(getType()=="INT"){
                   address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "func", id, address);
                    }else{
                        Symbol.modify(type, "func", id, address);
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

class Number_INT
    : public Expression
{
private:
    double value;
public:
    Number_INT(double _value) {
        value = _value;
    }
    double getValue() const
    { return value; }
    virtual void print(std::ostream &dst) const override
    { dst<<value; }
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return value;
    }
    virtual const std::string getDataType() const override {
        return "INT";
    }
    virtual void CompileRec(std::string destReg) const override {
        std::cout << "addi " << destReg << ", $0, " << value << std::endl;
    }
};

class Number_DOUBLE
    : public Expression
{
private:
    double value;
public: 
    Number_DOUBLE(double _value) {
        value = _value;
    }
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
    virtual const std::string getDataType() const override {
        return "FLOAT";
    }
    virtual void CompileRec(std::string destReg) const override {
        std::cout << "li.s " << destReg << ", " << value << std::endl;
    }
};

#endif
