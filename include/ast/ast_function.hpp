#ifndef ast_function_hpp
#define ast_function_hpp

#include "ast_primitives.hpp"
#include "ast_statement.hpp"
#include "ast_expression.hpp"

class Function
{
private:
    
    Variable *name; // function name
    ExpressionPtr arg; // single argument
    StatementPtr statements;
protected:
    Function(Variable *_name, ExpressionPtr _arg = nullptr, StatementPtr _statements = nullptr)
        : name(_name)
        , arg(_arg) 
        , statements(_statements)
    {}
public:
    virtual ~Function()
    {
        delete name,
        delete arg,
        delete statements;
    }

    virtual const char *getFunction() const =0;

    ExpressionPtr getArg() const
    { return arg; }

    void print(std::ostream &dst) const
    {
        name->print(dst);
        dst<<"( ";
        dst<<" )";
        statements->print(dst);
    }

};
#endif