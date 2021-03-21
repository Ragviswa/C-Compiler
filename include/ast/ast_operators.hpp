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
    Operator(ExpressionPtr _left, ExpressionPtr _right)
        : left(_left)
        , right(_right)
    {}
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
    
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override 
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl+vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "add " << destReg << ", $t0, $t1" << std::endl;
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
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "sub " << destReg << ", $t0, $t1" << std::endl;
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

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        double vl=getLeft()->evaluate(bindings);
        double vr=getRight()->evaluate(bindings);
        return vl*vr;
    }

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "mul " << destReg << ", $t0, $t1" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "div $t0, $t1" << std::endl;
        std::cout << "mfhi " << destReg << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "div $t0, $t1" << std::endl;
        std::cout << "mflo " << destReg << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "xor " << destReg << ", $t0, $t1" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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

    virtual void CompileRec(std::string destReg) const override {
        getLeft()->CompileRec("$t0");
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getRight()->CompileRec("$t1");
        std::cout << "lw $t0, -4($sp)" << std::endl;
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
        std::cout << "sw $t0, -4($sp)" << std::endl;
        getLeft()->CompileRec("$t1");
        std::cout << "sw $t1, -8($sp)" << std::endl;
        getRight()->CompileRec("$t2");
        std::cout << "lw $t0, -4($sp)" << std::endl;
        std::cout << "lw $t1, -8($sp)" << std::endl;
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
