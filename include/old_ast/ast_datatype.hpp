#ifndef ast_datatype_hpp
#define ast_datatype_hpp

#include "ast_expression.hpp"
#include "ast_primitives.hpp"
#include "ast_statement.hpp"

class Declare
    : public Statement
{
private:
    // TO-DO: specify data type later
    Integer name; // Variable name
protected:
    Function(Integer _name)
        : name(_name)
    {}
public:
    virtual ~Declare()
    {
        delete val;
    }

    virtual const char *getOpcode() const =0;

    Variable getName() const
    { return name.getId();}

    virtual void print(std::ostream &dst) const override
    {
        name.getId().print(dst);
        dst<<";";
    }

};

class Integer
    : public Declare
{
protected:
    virtual const char *getOpcode() const override
    { return ""; }
public:
    Integer(Integer _name)
        : Declare(_name)
    {}

};