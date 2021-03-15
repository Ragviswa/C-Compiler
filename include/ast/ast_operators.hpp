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

    virtual const char *getInstr() const;

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

    virtual void CompileRec(std::string destReg) const override {
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        left->CompileRec(srcRegA);
        right->CompileRec(srcRegB);
        std::cout << getInstr() << " " << destReg << " " << srcRegA << " " << srcRegB << std::endl;
    }
};

// Mathematical Operators
class AddOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "+"; }

    virtual const char *getInstr() const override
    { return "add"; }
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
};

class SubOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "-"; }
    virtual const char *getInstr() const override
    { return "sub"; }
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
};

class MulOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "*"; }
    virtual const char *getInstr() const override
    { return "mul"; }
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::cout << "div " << srcRegA << " " << srcRegB << std::endl; // quotient stored in hi, remainder stored in lo
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::cout << "div " << srcRegA << " " << srcRegB << std::endl; // quotient stored in hi, remainder stored in lo
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::cout << "slt " << destReg << " " << srcRegB << " " << srcRegA << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_one = makeName("set_one");
        std::cout << "beq " << srcRegA << " " << srcRegB << " " << set_one << std::endl;
        std::cout << "slt " << destReg << " " << srcRegB << " " << srcRegA << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_one << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::cout << "slt " << destReg << " " << srcRegA << " " << srcRegB << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_one = makeName("set_one");
        std::cout << "beq " << srcRegA << " " << srcRegB << " " << set_one << std::endl;
        std::cout << "slt " << destReg << " " << srcRegA << " " << srcRegB << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_one << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_one = makeName("set_one");
        std::cout << "beq " << srcRegA << " " << srcRegB << " " << set_one << std::endl;
        std::cout << "add " << destReg << " $0 $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_one << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_one = makeName("set_one");
        std::cout << "bne " << srcRegA << " " << srcRegB << " " << set_one << std::endl;
        std::cout << "add " << destReg << " $0 $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_one << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_zero = makeName("set_zero");
        std::cout << "beq " << srcRegA << " $0 " << set_zero << std::endl;
        std::cout << "beq " << srcRegB << " $0 " << set_zero << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_zero << std::endl;
        std::cout << "add " << destReg << " $0 $0" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        getLeft()->CompileRec(srcRegA);
        getRight()->CompileRec(srcRegB);
        std::string set_one = makeName("set_one");
        std::cout << "bne " << srcRegA << " $0 " << set_one << std::endl;
        std::cout << "bne " << srcRegB << " $0 " << set_one << std::endl;
        std::cout << "add " << destReg << " $0 $0" << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << set_one << std::endl;
        std::cout << "addi " << destReg << " $0 1" << std::endl;
        std::cout << ":" << exit << std::endl;
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
        std::string srcRegA = makeName("srcRegA");
        std::string srcRegB = makeName("srcRegB");
        std::string srcRegC = makeName("srcRegC");
        getcond()->CompileRec(srcRegA);
        getLeft()->CompileRec(srcRegB);
        getRight()->CompileRec(srcRegC);
        std::string else_stat = makeName("else_stat");
        std::cout << "beq " << srcRegA << " $0 " << else_stat <<std::endl;
        std::cout << "add " << destReg << " $0 " << srcRegB << std::endl;
        std::string exit = makeName("exit");
        std::cout << "jump " << exit << std::endl;
        std::cout << ":" << else_stat << std::endl;
        std::cout << "add " << destReg << " $0 " << srcRegC << std::endl;
        std::cout << ":"<<exit<<std::endl;
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
