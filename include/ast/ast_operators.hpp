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
    { return "/"; }
public:
    ModOperator(ExpressionPtr _left, ExpressionPtr _right)
        : Operator(_left, _right)
    {}

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

//Assign Expression
class Assign
    : public Expression
{
private:
    // TO-DO: specify data type later
    Integer name;
    ExpressionPtr val; // Assigning Val
protected:
    Function(Integer _name, ExpressionPtr _val = nullptr)
        : name(_name)
        , val(_val) 
    {}
public:
    virtual ~Assign()
    {
        delete val;
    }

    virtual const char *getOpcode() const =0;

    Integer getName() const
    { return name.getId();}

    ExpressionPtr getArg() const
    { return val; }

    virtual void print(std::ostream &dst) const override
    {
        name.getId().print(dst);
        dst<<getOpcode();
        val->print(dst);
        dst<<";";
    }

};

class IntegerAssign
    : public Assign
{
protected:
    virtual const char *getOpcode() const override
    { return "="; }
public:
    IntegerAssign(Integer _name, ExpressionPtr _val)
        : Assign(_name, _val)
    {}

};

#endif
