#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
#include <math.h>

#include "symbol_table.hpp"

enum TypeDef{
  INT,
  FLOAT,
  DOUBLE,
  CHAR,
  ENUM
};

enum DeclType{
  CALL,
  ASSIGN,
  DECL,
  ARG,
  STRUCT,
  TYPEDEF
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
    bool addrOf;
    std::string sid;
    double globalval;
public:
//This constructor does not feel right, it's a placeholder.
    Variable() {
    }
    
    Variable(const std::string *_id, bool _addrOf = false) {
        VarType = CALL;
        id = *_id;
        addrOf = _addrOf;
    }

    Variable(const std::string *_id, std::string *_AssignOp, ExpressionPtr _Expr) {
        VarType = ASSIGN;
        id = *_id;
        assignop = *_AssignOp;
        Expr = _Expr;
    }

    Variable(const std::string *_sid, const std::string *_id, DeclType _form) {
        VarType = _form;
        type = "STRUCT";
        sid = *_sid;
        id = *_id;
    }

    Variable(TypeDef _type, const std::string *_id, DeclType _form, ExpressionPtr _Expr = nullptr, double _globalval = 0) {
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
                case CHAR:
                    type = "CHAR";
                    id = *_id;
                    break;
                default:
                    type = "something went wrong";
            }
        }else if(_form == STRUCT){
            VarType = STRUCT;
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
                case CHAR:
                    type = "CHAR";
                    id = *_id;
                    break;
                default:
                    type = "something went wrong";
            }
        }else if(_form == DECL){
            globalval = _globalval;
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
                case CHAR:
                    type = "CHAR";
                    id = *_id;
                    Expr = _Expr;
                    break;
                case ENUM:
                    type = "ENUM";
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
        if(!type.empty()) {
            return type;
        }
        type = Symbol.getType(id);
        return type;
    }

    virtual const int getDataLength() const override {
        return Symbol.getSize(id);
    }

    virtual const std::string getDataFormat() const override {
        if(VarType == ARG) {
            return "var";
        }
        std::string format = Symbol.getFormat(id);
        return format;
    }

    virtual void CompileRec(std::string destReg) const override{
        std::string format;
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                format = Symbol.getFormat(id);
                if(format == "ptr") {
                    if(addrOf) { // &p
                        std::cout << "addi " << destReg << ", $fp, -" << address << std::endl;
                    }else { // p
                        std::cout << "lw " << destReg << ", -" << address << "($fp)" << std::endl;
                    }
                }else if(format == "var") {
                    if(addrOf) {
                        std::cout << "addi " << destReg << ", $fp, -" << address << std::endl;
                    }else {
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
                        else if(type == "CHAR") {
                            std::cout << "lb " << destReg << ", -" << address << "($fp)" << std::endl;
                        }
                    }
                }else if(format == "gvar") {
                    if(addrOf) {
                        std::cout << "addi " << destReg << ", $fp, -" << address << std::endl;
                    }else{
                        if(type == "INT") {
                            std::cout << "lw " << destReg << ", " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 " << destReg << ", " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 " << destReg << ", " << id << std::endl;
                            std::cout << "lwc1 " << destReg[0] << destReg[1] << (int)destReg[2]-48+1 << ", " << std::stoi(address)-4 << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lb " << destReg << ", " << id << std::endl;
                        }
                    }
                }else if(format == "enum"){
                    std::cout << "addiu " << destReg << ", $0, " << address << std::endl;
                }
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                format = Symbol.getFormat(id);
                if(format == "ptr") {
                    if(assignop == "=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "+=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        if(type == "CHAR") {
                            std::cout << "add $t1, $t1, $t0";
                        }
                        else if(type == "DOUBLE") {
                            for(int i = 0; i < 8; i++) {
                                std::cout << "add $t1, $t1, $t0" << std::endl;
                            }
                        } else {
                            for(int i = 0; i < 4; i++) {
                                std::cout << "add $t1, $t1, $t0" << std::endl;
                            }
                        }
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "-=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        if(type == "CHAR") {
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            for(int i = 0; i < 8; i++) {
                                std::cout << "sub $t1, $t1, $t0" << std::endl;
                            }
                        } else {
                            for(int i = 0; i < 4; i++) {
                                std::cout << "sub $t1, $t1, $t0" << std::endl;
                            }
                        }
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "/=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mflo $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "*=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "mul $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "%=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mfhi $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "<<=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "sllv $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == ">>=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "srlv $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "^=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "xor $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "&=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "and $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "|=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "or $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "++") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        if(type == "CHAR") {
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "addi $t0, $t0, 8" << std::endl;
                        } else {
                            std::cout << "addi $t0, $t0, 4" << std::endl;
                        }
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "--") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        if(type == "CHAR") {
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "addi $t0, $t0, -8" << std::endl;
                        } else {
                            std::cout << "addi $t0, $t0, -4" << std::endl;
                        }
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                }else if(format == "var") {
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sb $t0, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                            std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                            std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "%="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "<<="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == ">>="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "^="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "&="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "|="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "--") {
                        if(type == "INT") {
                            std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi.s $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "lwc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                            std::cout << "addi.d $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "swc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }
                }else if(format == "gvar") {
                    if(assignop == "="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sw $t0, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "swc1 $f0, " << id << std::endl;  
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "swc1 $f0, " << id << std::endl;
                            std::cout << "swc1 $f1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sb $t0, " << id << std::endl;
                        }
                        else {
                            std::cout << getType() << std::endl;
                            std::cout << "ERROR: type missing" << std::endl;
                        }
                    }else if(assignop == "+="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "add.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "lwc1 $f3, " << id << std::endl;
                            std::cout << "add.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                            std::cout << "swc1 #f3, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "-="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "sub.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "lwc1 $f3, " << id << std::endl;
                            std::cout << "sub.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                            std::cout << "swc1 #f3, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "/="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "div.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "lwc1 $f3, " << id << std::endl;
                            std::cout << "div.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                            std::cout << "swc1 #f3, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "*="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "mul.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, " << id << std::endl;
                            std::cout << "lwc1 $f3, " << id << std::endl;
                            std::cout << "mul.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, " << id <<std::endl;
                            std::cout << "swc1 #f3, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "%="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "<<="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == ">>="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "^="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "&="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "|="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, " << id << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, " << id << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "++") {
                        if(type == "INT") {
                            std::cout << "lw $t0, " << id <<std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "sw $t0, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, " << id << std::endl;
                            std::cout << "addi.s $f0, $f0, 1" << std::endl;
                            std::cout << "swc1 $f0, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, " << id << std::endl;
                            std::cout << "lwc1 $f1, " << id << std::endl;
                            std::cout << "addi.d $f0, $f0, 1" << std::endl;
                            std::cout << "swc1 $f0, " << id << std::endl;
                            std::cout << "swc1 $f1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, " << id << std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }else if(assignop == "--") {
                        if(type == "INT") {
                            std::cout << "lw $t0, " << id << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "sw $t0, " << id << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, " << id << std::endl;
                            std::cout << "addi.s $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, " << id << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, " << id << std::endl;
                            std::cout << "lwc1 $f1, " << id << std::endl;
                            std::cout << "addi.d $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, " << id << std::endl;
                            std::cout << "swc1 $f1, " << id << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, " << id << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, " << id << std::endl;
                        }
                    }
                }else if(format == "var") {
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sb $t0, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                            std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                            std::cout << "lwc1 $f2, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "%="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "<<="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == ">>="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "^="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "&="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "|="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, -" << address << "($fp)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
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
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }else if(assignop == "--") {
                        if(type == "INT") {
                            std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi.s $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "lwc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                            std::cout << "addi.d $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, -" << address << "($fp)" << std::endl;
                            std::cout << "swc1 $f1, -" << std::stoi(address)+4 << "($fp)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, -" << address << "($fp)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, -" << address << "($fp)" << std::endl;
                        }
                    }
                }
                break;
            case DECL:
                if(Symbol.getScope()==0 && (getType() != "ENUM" && getType() != "STRUCT")){
                    double globalval = getExpr()->evaluate();
                    if(getType()=="INT"){
                        Symbol.insert(type, "gvar", id, address);
                        std::cout << id << ": " << ".word "  << globalval <<std::endl;
                    }else if(getType()=="FLOAT"){
                        Symbol.insert(type, "gvar", id, address);
                        std::cout << id << ": " << ".float "  << globalval <<std::endl;
                    }else if(getType()=="CHAR"){
                        Symbol.insert(type, "gvar", id, address);
                        std::cout << id << ": " << ".byte "  << globalval <<std::endl;
                    }
                }
                else if(getType()=="INT"){
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
                        Symbol.modify(type, "gvar", id, address);
                    }
                }
                else if(getType() == "DOUBLE") {
                    std::cout << "addi $sp, $sp, -8" << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                    address = std::to_string(StackPointer.getIncr()-4); // address loading upper part
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
                        Symbol.modify(type, "gvar", id, address);
                    }
                }
                else if(getType() == "CHAR") {
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
                        getExpr()->CompileRec("$t0");
                        std::cout << "andi $t0, $t0, 0x00ff" << std::endl;
                        std::cout << "sb $t0, -" << address << "($fp)" << std::endl;
                    }
                    if(Symbol.getScope()==0) {
                        std::cout << ".global " << getId() << std::endl;
                        Symbol.modify(type, "gvar", id, address);
                    }
                }
                else if(getType() == "ENUM") {
                    if(Expr!=nullptr){
                        int enumval = getExpr()->evaluate();
                        StackPointer.setEnumdef(enumval);
                    }
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("INT", "enum", id, std::to_string(StackPointer.getEnumdef()));
                    }else{
                        Symbol.modify("INT", "enum", id, std::to_string(StackPointer.getEnumdef()));
                    }
                }
                else if(getType() == "STRUCT"){
                    int amount = std::stoi(Symbol.lookUp(sid));
                    std::cout << "addi $sp, $sp, -" << amount << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+amount);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+amount);
                    std::vector<std::string> StructMembers = Symbol.getStructMemb("struct"+sid);
                    int offset = 0;
                    int size = 0;
                    for(int i = 0; i < StructMembers.size(); i++){
                        int address = StackPointer.getIncr()-std::stoi(Symbol.lookUp(StructMembers[i]));
                        if(Symbol.getType(StructMembers[i]).substr(0, 3) == "arr"){
                                if(Symbol.lookUp(id+StructMembers[i]) == "Error: undefined reference"){
                                    Symbol.insert(Symbol.getType(StructMembers[i]).substr(3, Symbol.getType(StructMembers[i]).length()-3), "array", id+StructMembers[i], std::to_string(address));
                                }else{
                                    Symbol.modify(Symbol.getType(StructMembers[i]).substr(3, Symbol.getType(StructMembers[i]).length()-3), "array", id+StructMembers[i], std::to_string(address));
                                }
                        }else{
                                if(Symbol.lookUp(id+StructMembers[i]) == "Error: undefined reference"){
                                    Symbol.insert(Symbol.getType(StructMembers[i]), "var", id+StructMembers[i], std::to_string(address));
                                }else{
                                    Symbol.modify(Symbol.getType(StructMembers[i]), "var", id+StructMembers[i], std::to_string(address));
                                }
                        }
                        if(Symbol.getType(StructMembers[i])=="CHAR"){
                            size+=1;
                        }else if(Symbol.getType(StructMembers[i])=="INT"||Symbol.getType(StructMembers[i])=="FLOAT"){
                            size+=4;
                        }else if(Symbol.getType(StructMembers[i])=="DOUBLE"){
                            size+=8;
                        }
                    }
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("STRUCT", std::to_string(size), id, std::to_string(StackPointer.getIncr()-amount));
                    }else{
                        Symbol.modify("STRUCT", std::to_string(size), id, std::to_string(StackPointer.getIncr()-amount));
                    }
                }
                break;
            case ARG:
                if(getType()=="INT"){
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    if(StackPointer.getArgc()<4){
                        std::cout << "sw $a" << StackPointer.getArgc() << ", 0($sp)"<<  std::endl;
                    }else{
                        std::cout << "lw $t0, " << StackPointer.getArgSize() <<  "($fp)"<<  std::endl;
                        std::cout << "sw $t0, 0($sp)" <<  std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
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
                    }else {
                        std::cout << "lwc1 $f0, " << StackPointer.getArgSize() << "($fp)" << std::endl;
                        std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
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
                    }else {
                        std::cout << "lwc1 $f0 " << StackPointer.getArgSize() << "($fp)" << std::endl;
                        std::cout << "lwc1 $f1, " << StackPointer.getArgSize()-4 << "($fp)" << std::endl;
                        std::cout << "swc1 $f0, 0($sp)" << std::endl;
                        std::cout << "swc1 $f1, 4($sp)" << std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+8);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Symbol.getScope()==0){
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                else if(getType() == "CHAR") {
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    if(StackPointer.getArgc()<4){
                        std::cout << "sb $a" << StackPointer.getArgc() << ", 0($sp)"<<  std::endl;
                    }else {
                        std::cout << "lw $t0, " << StackPointer.getArgSize() << "($fp)" << std::endl;
                        std::cout << "sw $t0, 0($sp)" << std::endl;
                    }
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    address = std::to_string(StackPointer.getIncr());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "var", id, address);
                    }else{
                        Symbol.modify(type, "var", id, address);
                    }
                    if(Symbol.getScope()==0){
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                break;
            case STRUCT:
                if(getType()=="INT"){
                    StackPointer.setStructno(StackPointer.getStructno()+4);
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "FLOAT") {
                    StackPointer.setStructno(StackPointer.getStructno()+4);
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "DOUBLE") {
                    StackPointer.setStructno(StackPointer.getStructno()+8);
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "CHAR") {
                    StackPointer.setStructno(StackPointer.getStructno()+4);
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert(type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                break;
            case TYPEDEF:
                if(Symbol.lookUp(id) == "Error: undefined reference"){
                    Symbol.insert(type, "typedef", id, "0");
                }else{
                    Symbol.modify(type, "typedef", id, "0");
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

class EnumList
    : public Variable
{
private:
    Variable *enumvar;
    EnumList *enumlist = nullptr;
public:
    EnumList(Variable *_enumvar, EnumList *_enumlist = nullptr)
        : enumvar(_enumvar)
        , enumlist(_enumlist)
    {}

    virtual ~EnumList() {
        delete enumvar;
        delete enumlist;
    }
    Variable *getVar() const
    { return enumvar; }

    EnumList *getenumlist() const
    { return enumlist; }

    virtual void print(std::ostream &dst) const override
    {
        enumvar->print(dst);
        if(enumlist!=nullptr){
            dst << ", ";
            enumlist->print(dst);
            dst << "\n";
        }
    }

    virtual void CompileRec(std::string destReg) const override{
        getVar()->CompileRec(destReg);
        StackPointer.setEnumdef(StackPointer.getEnumdef()+1);
        if(enumlist!=nullptr){
            getenumlist()->CompileRec(destReg);
        }
        StackPointer.setEnumdef(0);
    }  
};

class EnumKeyword
    :public Variable
{
private:
    std::string id;
    mutable std::string address;
    EnumList *enumlist = nullptr;
public:
    EnumKeyword() {
    }
    
    EnumKeyword(const std::string *_id = nullptr, EnumList *_enumlist = nullptr) {
        id = *_id;
        enumlist = _enumlist;
    }

    const std::string getId() const
    { return id; }

    const std::string getAddr() const
    { return address; }

    ExpressionPtr getenumlist() const
    { return enumlist; }

    virtual void CompileRec(std::string destReg) const override{
        if(Symbol.lookUp(id) == "Error: undefined reference"){
            Symbol.insert("enum", "enum", id, std::to_string(StackPointer.getEnumdef()));
        }else{
            Symbol.modify("enum", "enum", id, std::to_string(StackPointer.getEnumdef()));
        }
        if(enumlist!=nullptr){
            getenumlist()->CompileRec(destReg);
        }

    }
};

class StructMemberList
    : public Variable
{
private:
    Variable *structmember;
    StructMemberList *structmemlist = nullptr;
public:
    StructMemberList(Variable *_structmember, StructMemberList *_structmemlist = nullptr)
        : structmember(_structmember)
        , structmemlist(_structmemlist)
    {}

    virtual ~StructMemberList() {
        delete structmember;
        delete structmemlist;
    }
    Variable *getVar() const
    { return structmember; }

    StructMemberList *getmemlist() const
    { return structmemlist; }

    virtual void print(std::ostream &dst) const override
    {
        structmember->print(dst);
        if(structmemlist!=nullptr){
            dst << "; ";
            dst << "\n";
            structmemlist->print(dst);
        }
    }

    virtual void CompileRec(std::string destReg) const override{
        getVar()->CompileRec(destReg);
        if(structmemlist!=nullptr){
            getmemlist()->CompileRec(destReg);
        }
    }  
};

class StructStorage
    :public Variable
{
private:
    std::string id;
    mutable std::string address;
    StructMemberList *structmemlist = nullptr;
public:
    StructStorage() {
    }
    
    StructStorage(const std::string *_id = nullptr, StructMemberList *_structmemlist = nullptr) {
        id = *_id;
        structmemlist = _structmemlist;
    }

    const std::string getId() const
    { return id; }

    const std::string getAddr() const
    { return address; }

    ExpressionPtr getstructmemlist() const
    { return structmemlist; }

    virtual void CompileRec(std::string destReg) const override{
        StackPointer.setStructno(0);
        if(Symbol.lookUp(id) == "Error: undefined reference"){
            Symbol.insert("structpar", "struct", id, "0");
        }else{
            Symbol.modify("structpar", "struct", id, "0");
        }
        StackPointer.setstruct(id);
        if(structmemlist!=nullptr){
            getstructmemlist()->CompileRec(destReg);
        }
        Symbol.modify("structpar", "struct", id, std::to_string(StackPointer.getStructno()));

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
    ExpressionListPtr list = nullptr;
    bool isPtr = false;
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

    Array(TypeDef _type, const std::string *_id, double _length, ExpressionListPtr _list = nullptr, bool _isPtr = false) {
        VarType = DECL;
        id = *_id;
        length = _length;
        list = _list;
        isPtr = _isPtr;
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

    Array(TypeDef _type, const std::string *_id, double _length, DeclType _vartype) {
        VarType = STRUCT;
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

    virtual const int getDataLength() const override {
        return Symbol.getSize(id);
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string format;
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                format = Symbol.getFormat(id);
                if(format == "array") {
                    getIndex()->CompileRec("$t0");
                    if(type == "INT") {
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lw " << destReg << ", 0($t1)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 " << destReg << ", 0($t1)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 " << destReg << ", 0($t1)" << std::endl;
                        std::cout << "lwc1 " << destReg[0] << destReg[1] << (int)destReg[2]-48+1 << ", -4($t1)" << std::endl;
                    }
                    else if(type == "CHAR") {
                        std::cout << "sll $t0, $t0, 0" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lb " << destReg << ", 0($t1)" << std::endl;
                    }
                }else if(format == "ptr") {
                    std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getIndex()->CompileRec("$t1");
                    std::cout << "lw $t0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "sll $t1, $t1, 2" << std::endl;
                    std::cout << "add $t0, $t0, $t1" << std::endl;
                    if(type == "INT") {
                        std::cout << "lw " << destReg << ", 0($t0)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        std::cout << "lwc1 " << destReg << ", 0($t0)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        std::cout << "lwc1 " << destReg << ", ($t0)" << std::endl;
                        std::cout << "lwc1 " << destReg[0]+destReg[1]+(int)destReg[2]-48+1 << ", 4($t0)" << std::endl;
                    }
                    else if(type == "CHAR") {
                        std::cout << "lb " << destReg << ", 0($t0)" << std::endl;
                        std::cout << "andi " << destReg << ", " << destReg << ", 0x00ff" << std::endl;
                    }
                }
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(assignop == "=") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; // 
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "sb $t0, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
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
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
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
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
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
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; //
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "add $t2, $t2, $t0" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "add $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "add.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                    }
                    if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f3, -4($t1)" << std::endl;
                        std::cout << "add.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                        std::cout << "swc1 $f3, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "-=") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; //
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "sub $t2, $t2, $t0" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "sub $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "sub.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                    }
                    if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f3, -4($t1)" << std::endl;
                        std::cout << "sub.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                        std::cout << "swc1 $f3, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "/=") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; //
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "div $t2, $t0" << std::endl;
                        std::cout << "mflo $t2" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "div $t2, $t0" << std::endl;
                        std::cout << "mflo $t2" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "div.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                    }
                    if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f3, -4($t1)" << std::endl;
                        std::cout << "div.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                        std::cout << "swc1 $f3, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "*=") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; //
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "mul $t2, $t2, $t0" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "mul $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "mul.s $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                    }
                    if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$f0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lwc1 $f2, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f3, -4($t1)" << std::endl;
                        std::cout << "mul.d $f2, $f2, $f0" << std::endl;
                        std::cout << "swc1 $f2, 0($t1)" << std::endl;
                        std::cout << "swc1 $f3, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "%=") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; //
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sb $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lb $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "div $t2, $t0" << std::endl;
                        std::cout << "mfhi $t2" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "div $t2, $t0" << std::endl;
                        std::cout << "mfhi $t2" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == "<<=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "sllv $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == ">>=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "srlv $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == "^=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "xor $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == "&=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "and $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == "|=") {
                    if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "addi $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t1, 0($sp)" << std::endl;
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, 0($sp)" << std::endl;
                        std::cout << "addi $sp, $sp, 4" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "or $t2, $t2, $t0" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                }
                else if(assignop == "++") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "addi $t2, $t2, 1" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "addi $t2, $t2, 1" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "subu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 $f0, 0($t1)" << std::endl;
                        std::cout << "addi.s $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 $f0, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f1, -4($t1)" << std::endl;
                        std::cout << "addi.d $f0, $f0, 1" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                        std::cout << "swc1 $f1, -4($t1)" << std::endl;
                    }
                }
                else if(assignop == "--") {
                    if(type == "CHAR") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 0" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lb $t2, 0($t1)" << std::endl;
                        std::cout << "addi $t2, $t2, -1" << std::endl;
                        std::cout << "sb $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "INT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lw $t2, 0($t1)" << std::endl;
                        std::cout << "addi $t2, $t2, -1" << std::endl;
                        std::cout << "sw $t2, 0($t1)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 2" << std::endl; // x2
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 $f0, 0($t1)" << std::endl;
                        std::cout << "addi.s $f0, $f0, -1" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        getIndex()->CompileRec("$t0");
                        std::cout << "sll $t0, $t0, 3" << std::endl; // x3
                        std::cout << "addi $t1, $fp, -" << address << std::endl; // loading address of head to t1
                        std::cout << "addu $t1, $t1, $t0" << std::endl;
                        std::cout << "lwc1 $f0, 0($t1)" << std::endl;
                        std::cout << "lwc1 $f1, -4($t1)" << std::endl;
                        std::cout << "addi.d $f0, $f0, -1" << std::endl;
                        std::cout << "swc1 $f0, 0($t1)" << std::endl;
                        std::cout << "swc1 $f1, -4($t1)" << std::endl;
                    }
                }
                break;
            case STRUCT:
                if(getType()=="INT"){
                    StackPointer.setStructno(StackPointer.getStructno()+4*getLength());
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("arr"+type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "FLOAT") {
                    StackPointer.setStructno(StackPointer.getStructno()+4*getLength());
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("arr"+type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "DOUBLE") {
                    StackPointer.setStructno(StackPointer.getStructno()+8*getLength());
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("arr"+type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                else if(getType() == "CHAR") {
                    StackPointer.setStructno(StackPointer.getStructno()+4*getLength());
                    address = std::to_string(StackPointer.getStructno());
                    if(Symbol.lookUp(id) == "Error: undefined reference"){
                        Symbol.insert("arr"+type, "struct"+StackPointer.getstruct(), "."+id, address);
                    }
                }
                break;
            case DECL:
                if(isPtr) {
                    std::cout << "addi $sp, $sp, -" << 4*getLength() << std::endl;
                    address = std::to_string((int)(StackPointer.getIncr()+4*getLength()));
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
                }else {
                    if(getType() == "CHAR") {
                        std::cout << "addi $sp, $sp, -" << getLength() << std::endl;
                        address = std::to_string((int)(StackPointer.getIncr()+getLength()));
                        StackPointer.setIncr(StackPointer.getIncr()+getLength());
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+getLength());
                        if(Symbol.lookUp(id) == "Error: undefined reference") {
                            Symbol.insert(type, "array", id, address);
                        }else {
                            Symbol.modify(type, "array", id, address);
                        }
                        if(Symbol.getScope() == 0) {
                            std::cout << ".global " << getId() << std::endl;
                        }
                    }
                    else if(getType()== "INT" || getType() == "FLOAT") {
                        std::cout << "addi $sp, $sp, -" << 4*getLength() << std::endl;
                        address = std::to_string((int)(StackPointer.getIncr()+4*getLength()));
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
                        address = std::to_string(StackPointer.getIncr()+8*getLength());
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
                }
                if(list != nullptr) {
                    ExpressionPtr curr = list->getArg();
                    ExpressionListPtr rest = list->getArgList();
                    int address_copy = std::stoi(address);
                    if(rest == nullptr) {
                        if(curr == nullptr) {
                            // nothing
                        }else {
                            if(getType() == "INT") {
                                curr->CompileRec("$t0");
                                std::cout << "sw $t0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 4;
                            }else if(getType() == "FLOAT") {
                                curr->CompileRec("$f0");
                                std::cout << "swc1 $f0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 4;
                            }else if(getType() == "DOUBLE") {
                                curr->CompileRec("$f0");
                                std::cout << "swc1 $f0, -" << address_copy << "($fp)" << std::endl;
                                std::cout << "swc1 $f1, -" << address_copy-4 << "($fp)" << std::endl;
                                address_copy = address_copy - 8;
                            }else if(getType() == "CHAR") {
                                curr->CompileRec("$t0");
                                std::cout << "sb $t0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 1;
                            }
                        }
                    }else {
                        while(rest != nullptr) {
                            if(getType() == "INT" || isPtr) {
                                curr->CompileRec("$t0");
                                std::cout << "sw $t0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 4;
                            }else if(getType() == "FLOAT") {
                                curr->CompileRec("$f0");
                                std::cout << "swc1 $f0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 4;
                            }else if(getType() == "DOUBLE") {
                                curr->CompileRec("$f0");
                                std::cout << "swc1 $f0, -" << address_copy << "($fp)" << std::endl;
                                std::cout << "swc1 $f1, -" << address_copy-4 << "($fp)" << std::endl;
                                address_copy = address_copy - 8;
                            }else if(getType() == "CHAR") {
                                curr->CompileRec("$t0");
                                std::cout << "sb $t0, -" << address_copy << "($fp)" << std::endl;
                                address_copy = address_copy - 1;
                            }
                            curr = rest->getArg();
                            rest = rest->getArgList();
                        }
                    }
                }
                break;
        }
    }
};

class Pointer
    :public Variable
{
private:
    mutable std::string type;
    std::string id;
    mutable std::string address;
    DeclType VarType;
    ExpressionPtr Expr; // address pointing to
    std::string assignop;
    bool deref;
    bool addrOf;
    std::string stringData = ""; // for char *c = "hello";
public:
    Pointer() {}

    Pointer(const std::string *_id, bool _deref = false, bool _addrOf = false) {
        VarType = CALL;
        id = *_id;
        deref = _deref;
        addrOf = _addrOf;
    }

    Pointer(const std::string *_id, std::string *_assignop, ExpressionPtr _Expr, bool _deref = false) {
        VarType = ASSIGN;
        id = *_id;
        assignop = *_assignop;
        Expr = _Expr;
        deref = _deref;
    }

    Pointer(const std::string *_id, const std::string *_stringData) { // char *p = "hello";
        VarType = DECL;
        id = *_id;
        stringData = _stringData->substr(1,_stringData->length()-2);
        type = "CHAR";
    }

    Pointer(TypeDef _type, const std::string *_id, DeclType _form, ExpressionPtr _Expr = nullptr) {
        if(_form == ARG) {
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
                case CHAR:
                    type = "CHAR";
                    id = *_id;
                    break;
                default:
                    type = "something went wrong";
            }
        }else if(_form == DECL) {
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
                case CHAR:
                    type = "CHAR";
                    id = *_id;
                    Expr = _Expr;
                    break;
                default:
                    type = "something went wrong";
            }
        }
    }

    const std::string getType() const {
        return type;
    }

    const std::string getId() const {
        return id;
    }

    const std::string getAddr() const {
        return address;
    }

    ExpressionPtr getExpr() const {
        return Expr;
    }

    virtual const std::string getDataType() const override {
        type = Symbol.getType(id);
        return type;
    }

    virtual const int getDataLength() const override {
        return Symbol.getSize(id);
    }

    virtual void CompileRec(std::string destReg) const override {
        switch(VarType) {
            case CALL:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(deref) { // *p
                    std::cout << "lw $t0, -" << address << "($fp)" << std::endl; // address of x
                    if(type == "INT") {
                        std::cout << "lw " << destReg << ", 0($t0)" << std::endl;
                    }
                    else if(type == "FLOAT") {
                        std::cout << "lwc1 " << destReg << ", 0($t0)" << std::endl;
                    }
                    else if(type == "DOUBLE") {
                        std::cout << "lwc1 " << destReg << ", 0($t0)" << std::endl;
                        std::cout << "lwc1 " << destReg[0]<<destReg[1]<<(int)destReg[2]-48+1 << ", -4($t0)" << std::endl;
                    }
                    else if(type == "CHAR") {
                        std::cout << "lb " << destReg << ", 0($t0)" << std::endl;
                    }
                }
                else if(addrOf) { // &p
                    std::cout << "addi " << destReg << ", $fp, -" << address << std::endl;
                }
                else { // p
                    std::cout << "lw " << destReg << ", -" << address << "($fp)" << std::endl;
                }
                break;
            case ASSIGN:
                address = Symbol.lookUp(id);
                type = Symbol.getType(id);
                if(deref) {
                    std::cout << "lw $t2, -" << address << "($fp)" << std::endl;
                    if(assignop == "="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sw $t0, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;  
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;
                            std::cout << "swc1 $f1, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "sb $t0, 0($t2)" << std::endl;
                        }
                        else {
                            std::cout << getType() << std::endl;
                            std::cout << "ERROR: type missing" << std::endl;
                        }
                    }else if(assignop == "+="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "add.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f3, -4($t2)" << std::endl;
                            std::cout << "add.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                            std::cout << "swc1 #f3, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "add $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "-="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "sub.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f3, -4($t2)" << std::endl;
                            std::cout << "sub.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                            std::cout << "swc1 #f3, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "sub $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "/="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "div.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, -" << address << "($fp)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f3, -4($t2)" << std::endl;
                            std::cout << "div.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                            std::cout << "swc1 #f3, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mflo $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "*="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "mul.s $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            getExpr()->CompileRec("$f0");
                            std::cout << "lwc1 $f2, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f3, -4($t2)" << std::endl;
                            std::cout << "mul.d $f2, $f2, $f0" << std::endl;
                            std::cout << "swc1 $f2, 0($t2)" << std::endl;
                            std::cout << "swc1 #f3, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "mul $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "%="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "div $t1, $t0" << std::endl;
                            std::cout << "mfhi $t1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "<<="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "sllv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == ">>="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "srlv $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "^="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "xor $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "&="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "and $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "|="){
                        if(type == "INT") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lw $t1, 0($t2)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "sw $t1, 0($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            getExpr()->CompileRec("$t0");
                            std::cout << "lbu $t1, 0($t2)" << std::endl;
                            std::cout << "or $t1, $t1, $t0" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "++") {
                        if(type == "INT") {
                            std::cout << "lw $t0, 0($t2)" << std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "sw $t0, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, 0($t2)" << std::endl;
                            std::cout << "addi.s $f0, $f0, 1" << std::endl;
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f1, -4($t2)" << std::endl;
                            std::cout << "addi.d $f0, $f0, 1" << std::endl;
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;
                            std::cout << "swc1 $f1, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, 0($t2)" << std::endl;
                            std::cout << "addi $t0, $t0, 1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }else if(assignop == "--") {
                        if(type == "INT") {
                            std::cout << "lw $t0, 0($t2)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "sw $t0, 0($t2)" << std::endl;
                        }
                        else if(type == "FLOAT") {
                            std::cout << "lwc1 $f0, 0($t2)" << std::endl;
                            std::cout << "addi.s $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;
                        }
                        else if(type == "DOUBLE") {
                            std::cout << "lwc1 $f0, 0($t2)" << std::endl;
                            std::cout << "lwc1 $f1, -4($t2)" << std::endl;
                            std::cout << "addi.d $f0, $f0, -1" << std::endl;
                            std::cout << "swc1 $f0, 0($t2)" << std::endl;
                            std::cout << "swc1 $f1, -4($t2)" << std::endl;
                        }
                        else if(type == "CHAR") {
                            std::cout << "lbu $t0, 0($t2)" << std::endl;
                            std::cout << "addi $t0, $t0, -1" << std::endl;
                            std::cout << "andi $t1, $t1, 0x00ff" << std::endl;
                            std::cout << "sb $t1, 0($t2)" << std::endl;
                        }
                    }
                }
                else { // p = &a;
                    if(assignop == "=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "+=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "add $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "-=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "sub $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "/=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mflo $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "*=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "mul $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "%=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "div $t1, $t0" << std::endl;
                        std::cout << "mfhi $t1" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "<<=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "sllv $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == ">>=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "srlv $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "^=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "xor $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "&=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "and $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "|=") {
                        getExpr()->CompileRec("$t0");
                        std::cout << "lw $t1, -" << address << "($fp)" << std::endl;
                        std::cout << "or $t1, $t1, $t0" << std::endl;
                        std::cout << "sw $t1, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "++") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        std::cout << "addi $t0, $t0, 1" << std::endl;
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                    else if(assignop == "--") {
                        std::cout << "lw $t0, -" << address << "($fp)" << std::endl;
                        std::cout << "addi $t0, $t0, -1" << std::endl;
                        std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                    }
                }
                break;
            case DECL:
                std::cout << "addi $sp, $sp, -4" << std::endl;
                StackPointer.setIncr(StackPointer.getIncr()+4);
                StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                address = std::to_string(StackPointer.getIncr());
                if(Symbol.lookUp(id) == "Error: undefined reference") {
                    Symbol.insert(type, "ptr", id, address);
                }else {
                    Symbol.modify(type, "ptr", id, address);
                }
                if(Expr != nullptr) {
                    getExpr()->CompileRec("$t0");
                    std::cout << "sw $t0, -" << address << "($fp)" << std::endl;
                }
                if(Symbol.getScope() == 0) {
                    std::cout << ".global " << getId() << std::endl;
                }
                if(!stringData.empty()) {
                    std::cout << "addi $sp, $sp, -" << 4*stringData.length() << std::endl;
                    std::cout << "sw $sp, -" << address << "($fp)" << std::endl;
                    StackPointer.setIncr(StackPointer.getIncr()+4*stringData.length());
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4*stringData.length());
                    if(Symbol.lookUp(id+"[]") == "Error: undefined reference") {
                        Symbol.insert(type, "ptr", id+"[]", address);
                    }else {
                        Symbol.modify(type, "ptr", id+"[]", address);
                    }
                    for(int count = 0; count < stringData.length(); count++) {
                        std::cout << "li $t0, " << (int) stringData[count] << std::endl;
                        std::cout << "sb $t0, " << 4*count << "($sp)" << std::endl;
                    }
                    if(Symbol.getScope() == 0) {
                        std::cout << ".global " << getId() << std::endl;
                    }
                }
                break;
            case ARG:
                std::cout << "addi $sp, $sp, -4" << std::endl;
                if(StackPointer.getArgc()<4){
                    std::cout << "sw $a" << StackPointer.getArgc() << ", 0($sp)" << std::endl;
                }else{
                    std::cout << "lw $t0, " << StackPointer.getArgSize() <<  "($fp)" <<  std::endl;
                    std::cout << "sw $t0, 0($sp)" <<  std::endl;
                }
                StackPointer.setIncr(StackPointer.getIncr()+4);
                StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                address = std::to_string(StackPointer.getIncr());
                if(Symbol.lookUp(id) == "Error: undefined reference") {
                    Symbol.insert(type, "ptr", id, address);
                }else {
                    Symbol.insert(type, "ptr", id, address);
                }
                if(Symbol.getScope() == 0) {
                    std::cout << ".global " << getId() << std::endl;
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
            case CHAR:
                type = "CHAR";
                id = *_id;
                break;
            default:
                type = "something went wrong";
        }
    }
    
    const std::string getType() const
    { return type; }

    virtual const std::string getDataType() const override {
        return Symbol.getType(id);
    }

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
                    std::cout << "add " << destReg << ", $0, $v0" << std::endl; 
                    std::cout << "addiu $sp, $sp, " << StackPointer.getArgSize() << std::endl; 
                    StackPointer.setIncr(StackPointer.getIncr()-StackPointer.getArgSize());
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()-StackPointer.getArgSize());
                    StackPointer.setArgc(0);
                    StackPointer.setArgSize(0);
                }else{
                    address = std::to_string(std::stoi(Symbol.lookUp(id))+44);
                    if(Args!=nullptr){
                        Args->CompileRec(destReg, address);
                    }
                    std::cout << "jal " << id << std::endl;
                    std::cout << "addiu $sp, $sp, " << StackPointer.getArgSize() << std::endl; 
                    StackPointer.setIncr(StackPointer.getIncr()-StackPointer.getArgSize());
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()-StackPointer.getArgSize());
                    StackPointer.setArgc(0);
                    StackPointer.setArgSize(0);
                    if(getDataType() == "INT") {
                        std::cout << "add " << destReg << ", $v0, $0" << std::endl;
                    }
                    else if(getDataType() == "FLOAT") {
                        std::cout << "addi.s " << destReg << ", $f0, 0" << std::endl;
                    }
                    else if(getDataType() == "DOUBLE") {
                        std::cout << "addi.d " << destReg << ", $f0, 0" << std::endl;
                    }
                    else if(getDataType() == "CHAR") {
                        std::cout << "add " << destReg << ", $v0, $0" << std::endl;
                        std::cout << "andi " << destReg << ", " << destReg << ", 0x00ff" << std::endl;
                    }
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
        std::string a_format = getVar()->getDataFormat();
        std::string a_type = getVar()->getDataType();
        if(a_format == "ptr" || a_format == "array") {
            StackPointer.setArgSize(StackPointer.getArgSize()+4);
        }else if(a_format == "var") {
            if(a_type == "INT" || a_type == "FLOAT" || a_type == "CHAR") {
                StackPointer.setArgSize(StackPointer.getArgSize()+4);
            }else if(a_type == "DOUBLE") {
                StackPointer.setArgSize(StackPointer.getArgSize()+8);
            }
        }
        if(declarationList!=nullptr){
            getdecllist()->CompileRec(destReg);
        }
        StackPointer.setArgc(0);
        StackPointer.setArgSize(0);
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
    virtual int evaluate() const override
    {
        return value;
    }
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

class SingleChar
    : public Expression
{
private:
    char character; // char x = 'c';
    std::string multiChar; // char *x="hello";
public:
    SingleChar(const std::string *input) {
        if((*input)[0] == '\'') {
            character = (*input)[1];
        }
    }
    char getValue() const
    { return character; }
    virtual void print(std::ostream &dst) const override
    {
        dst<<character;
    }
    virtual const std::string getDataType() const override {
        return "CHAR";
    }
    virtual void CompileRec(std::string destReg) const override {
        std::cout << "li " << destReg << ", " << (int) character << std::endl;
    }
};

class String 
    : public Expression
{
private:
    std::string str;
public:
    String(const std::string *_str) {
        str = _str->substr(1,_str->length()-2);
    }
    const std::string getString() {
        return str;
    }
    virtual const std::string getDataType() const override {
        return "CHAR";
    }
    virtual void CompileRec(std::string destReg) const override{
        std::cout << "addi $sp, $sp, -" << (int)(ceil((double)str.length()/4)*4) << std::endl;
        for(int count = 0; count < str.length(); count++) {
            std::cout << "li $t0, " << (int) str[count] << std::endl;
            std::cout << "sb $t0, " << count << "($sp)" << std::endl;
        }
        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+(int)(ceil((double)str.length()/4)*4));
        std::cout << "addi " << destReg << ", $sp, 0" << std::endl;
    }
};

class SizeOf
    : public Expression
{
private:
    mutable double value;
    ExpressionPtr expr = nullptr;
public:
    SizeOf() {
    }
    SizeOf(ExpressionPtr _expr) {
        expr = _expr;
    }
    SizeOf(TypeDef _type) {
        switch(_type) {
            case INT:
                value = 4;
                break;
            case FLOAT:
                value = 4;
                break;
            case DOUBLE:
                value = 8;
                break;
            case CHAR:
                value = 1;
                break;
            default:
                value = -1;
        }
    }
    virtual void CompileRec(std::string destReg) const override {
        if(expr != nullptr) {
            const std::string type = expr->getDataType();
            if(type == "INT" || type == "FLOAT") {
                value = 4;
            }
            else if(type == "DOUBLE") {
                value = 8;
            }
            else if(type == "CHAR") {
                value = 1;
            }
            else if(type == "STRUCT") {
                value = std::stoi(expr->getDataFormat());
            }
        }
        std::cout << "addi " << destReg << ", $0, " << value << std::endl;
    }
};

#endif
