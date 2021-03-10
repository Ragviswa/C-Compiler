#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "ast_expression.hpp"
#include "ast_primitives.hpp"
#include "ast_statement.hpp"

class Assign
    : public Statement
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