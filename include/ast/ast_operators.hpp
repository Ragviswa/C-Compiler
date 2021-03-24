#ifndef ast_operators_hpp
#define ast_operators_hpp

#include <string>
#include <iostream>

// Base Class
class Operator
    : public Expression
{
private:
    ExpressionPtr left;
    ExpressionPtr right;
protected:
    Operator(ExpressionPtr _left, ExpressionPtr _right) {
        left = _left;
        right = _right;
    }

public:
    virtual ~Operator()
    {
        delete left;
        delete right;
    }

    virtual const char *getOpcode() const =0;

    ExpressionPtr getLeft() const
    { return left; }

    ExpressionPtr getRight() const
    { return right; }

    virtual const std::string getDataType() const override { 
        std::string leftDataType = getLeft()->getDataType();
        std::string rightDataType = getRight()->getDataType();
        if(leftDataType == "INT") {
            if(rightDataType == "INT") {
                return "INT";
            }
            else if(rightDataType == "FLOAT") {
                return "FLOAT";
            }
            else if(rightDataType == "DOUBLE") {
                return "DOUBLE";
            }
            else {
                return "ERROR: no datatype";
            }
        }
        else if(leftDataType == "FLOAT") {
            if(rightDataType == "INT" || rightDataType == "FLOAT") {
                return "FLOAT";
            }
            else if(rightDataType == "DOUBLE") {
                return "DOUBLE";
            }
        }
        else if(leftDataType == "DOUBLE") {
            return "DOUBLE";
        }
    }

    virtual void print(std::ostream &dst) const override
    {
        dst<<"( ";
        left->print(dst);
        dst<<" ";
        dst<<getOpcode();
        dst<<" ";
        right->print(dst);
        dst<<" )";
    }
};

// Mathematical Operators
class AddOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "+"; }

public:
    AddOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl+vr;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl+vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string leftType = getLeft()->getDataType();
        std::string rightType = getRight()->getDataType();
        std::string leftFormat = getLeft()->getDataFormat();
        if(leftFormat == "ptr") {
            getLeft()->CompileRec("$t0");
            std::cout << "addi $sp, $sp, -4" << std::endl;
            std::cout << "sw $t0, 0($sp)" << std::endl;
            getRight()->CompileRec("$t1");
            std::cout << "lw $t0, 0($sp)" << std::endl;
            std::cout << "addi $sp, $sp, 4" << std::endl;
            if(leftType != "DOUBLE") {
                for(int i = 0; i < 4; i++) {
                    std::cout << "add $t0, $t0, $t1" << std::endl;
                }
            }else{
                for(int i = 0; i < 8; i++) {
                    std::cout << "add $t0, $t0, $t1" << std::endl;
                }
            }
            std::cout << "addi " << destReg << ", $t0, 0" << std::endl;
        }
        else {
            if(leftType == "INT") {
                if(rightType == "INT") { // int + int
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$t1");
                    std::cout << "lw $t0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] == 'f') {
                        std::cout << "ERROR: wrong destReg" << std::endl;
                    }
                    std::cout << "add " << destReg << ", $t0, $t1" << std::endl;
                }
                else if(rightType == "FLOAT") { // int + float
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // int + double
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f0, $f0" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.d " << destReg << ", $f0, $f2" << std::endl;
                }
            }
            else if(leftType == "FLOAT") {
                if(rightType == "INT") {
                    getRight()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "FLOAT") {
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // float + double
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f0, $f0" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.d " << destReg << " , $f0, $f2" << std::endl; 
                }
            }
            else if(leftType == "DOUBLE") {
                if(rightType == "INT") {
                    getRight()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f2, $f2" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.d " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "FLOAT") { //double + float
                    getRight()->CompileRec("$f2");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f2, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1, $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f2, $f2" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.d " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // double + double
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -8" << std::endl;
                    std::cout << "swc1 $f1, 4($sp)" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f1, 4($sp)" << std::endl;
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 8" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "add.d " << destReg << ", $f0, $f2" << std::endl;
                }
            }
        }
    }
    
};

class SubOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "-"; }

public:
    SubOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl-vr;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl-vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string leftType = getLeft()->getDataType();
        std::string rightType = getRight()->getDataType();
        std::string leftFormat = getLeft()->getDataFormat();
        if(leftFormat == "ptr") {
            getLeft()->CompileRec("$t0");
            std::cout << "addi $sp, $sp, -4" << std::endl;
            std::cout << "sw $t0, 0($sp)" << std::endl;
            getRight()->CompileRec("$t1");
            std::cout << "lw $t0, 0($sp)" << std::endl;
            std::cout << "addi $sp, $sp, 4" << std::endl;
            if(leftType != "DOUBLE") {
                for(int i = 0; i < 4; i++) {
                    std::cout << "sub $t0, $t0, $t1" << std::endl;
                }
            }else{
                for(int i = 0; i < 8; i++) {
                    std::cout << "sub $t0, $t0, $t1" << std::endl;
                }
            }
            std::cout << "addi " << destReg << ", $t0, 0" << std::endl;
        }else {
            if(leftType == "INT") {
                if(rightType == "INT") { // int - int
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$t1");
                    std::cout << "lw $t0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] == 'f') {
                        std::cout << "ERROR: wrong destReg" << std::endl;
                    }
                    std::cout << "sub " << destReg << ", $t0, $t1" << std::endl;
                }
                else if(rightType == "FLOAT") { // int - float
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // int - double
                    getLeft()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f0, $f0" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.d " << destReg << ", $f0, $f2" << std::endl;
                }
            }
            else if(leftType == "FLOAT") {
                if(rightType == "INT") { // float - int
                    getRight()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "FLOAT") { // float - float
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.s " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // float - double
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f0, $f0" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.d " << destReg << " , $f0, $f2" << std::endl; 
                }
            }
            else if(leftType == "DOUBLE") {
                if(rightType == "INT") { // double - int
                    getRight()->CompileRec("$t0");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f2, $f2" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.d " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "FLOAT") { //double - float
                    getRight()->CompileRec("$f2");
                    std::cout << "addi $sp, $sp, -4" << std::endl;
                    std::cout << "swc1 $f2, 0($sp)" << std::endl;
                    getLeft()->CompileRec("$f0");
                    std::cout << "lwc1, $f2, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4" << std::endl;
                    std::cout << "cvt.d.s $f2, $f2" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.d " << destReg << ", $f0, $f2" << std::endl;
                }
                else if(rightType == "DOUBLE") { // double - double
                    getLeft()->CompileRec("$f0");
                    std::cout << "addi $sp, $sp, -8" << std::endl;
                    std::cout << "swc1 $f1, 4($sp)" << std::endl;
                    std::cout << "swc1 $f0, 0($sp)" << std::endl;
                    getRight()->CompileRec("$f2");
                    std::cout << "lwc1 $f1, 4($sp)" << std::endl;
                    std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 8" << std::endl;
                    if(destReg[1] != 'f') {
                        std::cout << "ERROR: Wrong destReg" << std::endl;
                    }
                    std::cout << "sub.d " << destReg << ", $f0, $f2" << std::endl;
                }
            }
        }
    }
};

class MulOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "*"; }
    
public:
    MulOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl*vr;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl*vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string leftType = getLeft()->getDataType();
        std::string rightType = getRight()->getDataType();
        if(leftType == "INT") {
            if(rightType == "INT") { // int * int
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$t1");
                std::cout << "lw $t0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] == 'f') {
                    std::cout << "ERROR: wrong destReg" << std::endl;
                }
                std::cout << "mul " << destReg << ", $t0, $t1" << std::endl;
            }
            else if(rightType == "FLOAT") { // int * float
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // int * double
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f0, $f0" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.d " << destReg << ", $f0, $f2" << std::endl;
            }
        }
        else if(leftType == "FLOAT") {
            if(rightType == "INT") { // float * int
                getRight()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "FLOAT") { // float * float
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // float * double
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f0, $f0" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.d " << destReg << " , $f0, $f2" << std::endl; 
            }
        }
        else if(leftType == "DOUBLE") {
            if(rightType == "INT") { // double * int
                getRight()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f2, $f2" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.d " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "FLOAT") { //double * float
                getRight()->CompileRec("$f2");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f2, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1, $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f2, $f2" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.d " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // double * double
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -8" << std::endl;
                std::cout << "swc1 $f1, 4($sp)" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f1, 4($sp)" << std::endl;
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 8" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "mul.d " << destReg << ", $f0, $f2" << std::endl;
            }
        }
    }
};

class DivOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "/"; }

public:
    DivOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl/vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string leftType = getLeft()->getDataType();
        std::string rightType = getRight()->getDataType();
        if(leftType == "INT") {
            if(rightType == "INT") { // int / int
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$t1");
                std::cout << "lw $t0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "div $t0, $t1" << std::endl;
                if(destReg[1] == 'f') {
                    std::cout << "ERROR: wrong destReg" << std::endl;
                }
                std::cout << "mflo " << destReg << std::endl;
            }
            else if(rightType == "FLOAT") { // int / float
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // int / double
                getLeft()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f0, $f0" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.d " << destReg << ", $f0, $f2" << std::endl;
            }
        }
        else if(leftType == "FLOAT") {
            if(rightType == "INT") { // float / int
                getRight()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "FLOAT") { // float / float
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.s " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // float / double
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f0, $f0" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.d " << destReg << " , $f0, $f2" << std::endl; 
            }
        }
        else if(leftType == "DOUBLE") {
            if(rightType == "INT") { // double / int
                getRight()->CompileRec("$t0");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1 $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f2, $f2" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.d " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "FLOAT") { //double / float
                getRight()->CompileRec("$f2");
                std::cout << "addi $sp, $sp, -4" << std::endl;
                std::cout << "swc1 $f2, 0($sp)" << std::endl;
                getLeft()->CompileRec("$f0");
                std::cout << "lwc1, $f2, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 4" << std::endl;
                std::cout << "cvt.d.s $f2, $f2" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.d " << destReg << ", $f0, $f2" << std::endl;
            }
            else if(rightType == "DOUBLE") { // double / double
                getLeft()->CompileRec("$f0");
                std::cout << "addi $sp, $sp, -8" << std::endl;
                std::cout << "swc1 $f1, 4($sp)" << std::endl;
                std::cout << "swc1 $f0, 0($sp)" << std::endl;
                getRight()->CompileRec("$f2");
                std::cout << "lwc1 $f1, 4($sp)" << std::endl;
                std::cout << "lwc1 $f0, 0($sp)" << std::endl;
                std::cout << "addi $sp, $sp, 8" << std::endl;
                if(destReg[1] != 'f') {
                    std::cout << "ERROR: Wrong destReg" << std::endl;
                }
                std::cout << "div.d " << destReg << ", $f0, $f2" << std::endl;
            }
        }
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl/vr;
    }
};

class ModOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "%"; }
public:
    ModOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl%vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "div $t0, $t1" << std::endl;
        std::cout << "mfhi " << destReg << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        int vl=getLeft()->evaluate(bindings);
        int vr=getRight()->evaluate(bindings);
        return vl%vr;
    }
};

// Relational Operators
class GreaterThanOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return ">"; }
public:
    GreaterThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl>vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "slt " << destReg << ", $t1, $t0" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl > vr;
    }
};

class GreaterThanEqualOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return ">="; }
public:
    GreaterThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl>=vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_one = makeName("set_one");
        std::cout << "beq $t0, $t1, " << set_one << std::endl;
        std::cout << "slt " << destReg << ", $t1, $t0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_one << ":" << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << exit << ":" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl >= vr;
    }
};

class LessThanOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return "<"; }
public:
    LessThanOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl<vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "slt " << destReg << ", $t0, $t1" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl < vr;
    }
};

class LessThanEqualOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return "<="; }
public:
    LessThanEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl<=vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_one = makeName("set_one");
        std::cout << "beq $t0, $t1, " << set_one << std::endl;
        std::cout << "slt " << destReg << ", $t0, $t1" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_one << ":" << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << exit << ":" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl <= vr;
    }
};

class EqualOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return "=="; }
public:
    EqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl==vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_one = makeName("set_one");
        std::cout << "beq $t0, $t1, " << set_one << std::endl;
        std::cout << "add " << destReg << ", $0, $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_one << ":" << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << exit << ":" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl == vr;
    }
};

class NotEqualOperator
    : public Operator
{
protected: 
    virtual const char *getOpcode() const override
        { return "!="; }
public:
    NotEqualOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl!=vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_one = makeName("set_one");
        std::cout << "bne $t0, $t1, " << set_one << std::endl;
        std::cout << "add " << destReg << ", $0, $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_one << ":" << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << exit << ":" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl = getLeft()->evaluate(bindings);
        double vr = getRight()->evaluate(bindings);
        return vl != vr;
    }
};
// Bitwise Operators

class AndOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "&"; }

public:
    AndOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl&vr;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        int vl=getLeft()->evaluate(bindings);
        int vr=getRight()->evaluate(bindings);
        return vl&vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "and " << destReg << ", $t0, $t1" << std::endl;
    }
    
};

class OrOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "|"; }

public:
    OrOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl|vr;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        int vl=getLeft()->evaluate(bindings);
        int vr=getRight()->evaluate(bindings);
        return vl|vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "or " << destReg << ", $t0, $t1" << std::endl;
    }
};

class XorOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "^"; }

public:
    XorOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl^vr;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl-vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "xor " << destReg << ", $t0, $t1" << std::endl;
    }
};

class LeftShift
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "<<"; }
public:
    LeftShift(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl<<vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "sllv " << destReg << ", $t0, $t1" << std::endl;
    }
};

class RightShift
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return ">>"; }
public:
    RightShift(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl>>vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::cout << "srlv " << destReg << ", $t0, $t1" << std::endl;
    }
};

// Logic Operators
class AndLogic
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "&&"; }
public:
    AndLogic(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl&&vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_zero = makeName("set_zero");
        std::cout << "beq $t0, $0, " << set_zero << std::endl;
        std::cout << "beq $t1, $0, " << set_zero << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_zero << ":" << std::endl;
        std::cout << "add " << destReg << ", $0, $0" << std::endl;
        std::cout << exit << ":" << std::endl;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl&&vr;
    }
};

class OrLogic
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "||"; }
public:
    OrLogic(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

    virtual int evaluate() const override
    {
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();
        return vl||vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -4" << std::endl;
        std::cout << "sw $t0, 0($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 4" << std::endl;
        std::string set_one = makeName("set_one");
        std::cout << "bne $t0, $0, " << set_one << std::endl;
        std::cout << "bne $t1, $0, " << set_one << std::endl;
        std::cout << "add " << destReg << ", $0, $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << set_one << ":" << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << exit << ":" << std::endl;
    }
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl||vr;
    }
};

// Ternary Operator
class TernaryOperator
    : public Expression
{
private:
    ExpressionPtr cond;
    ExpressionPtr left;
    ExpressionPtr right;
public:
    TernaryOperator(ExpressionPtr _cond, ExpressionPtr _left, ExpressionPtr _right)
        : cond(_cond)
        , left(_left)
        , right(_right)
    {}
    virtual ~TernaryOperator()
    {
        delete cond;
        delete left;
        delete right;
    }

    ExpressionPtr getcond() const
    { return cond; }

    ExpressionPtr getLeft() const
    { return left; }

    ExpressionPtr getRight() const
    { return right; }

    virtual int evaluate() const override
    {
        int vc=getcond()->evaluate();
        int vl=getLeft()->evaluate();
        int vr=getRight()->evaluate();

        return vc?vl:vr;
    }

    virtual void print(std::ostream &dst) const override
    {
        cond->print(dst);
        dst<<" ? ";
        left->print(dst);
        dst<<" : ";
        right->print(dst);
    }

    virtual void CompileRec(std::string destReg) const override {
        getcond()->CompileRec("$t0");
        std::cout << "addi $sp, $sp, -8" << std::endl;
        std::cout << "sw $t0, 4($sp)" << std::endl;
        getLeft()->CompileRec("$t1");
        std::cout << "sw $t1, 0($sp)" << std::endl;
        getRight()->CompileRec("$t2");
        std::cout << "lw $t0, 4($sp)" << std::endl;
        std::cout << "lw $t1, 0($sp)" << std::endl;
        std::cout << "addi $sp, $sp, 8" << std::endl;
        std::string else_stat = makeName("else_stat");
        std::cout << "beq $t0, $0, " << else_stat <<std::endl;
        std::cout << "add " << destReg << ", $0, $t1" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "j " << exit << std::endl;
        std::cout << else_stat << ":" << std::endl;
        std::cout << "add " << destReg << ", $0, $t2" << std::endl;
        std::cout << exit << ":" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        bool cond=getcond()->evaluate(bindings);
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return cond?vl:vr;
    }
};

#endif
