#ifndef ast_functions_hpp
#define ast_functions_hpp

#include "ast_expression.hpp"
#include "ast_primitives.hpp"
#include "ast_statement.hpp"

#include <vector>

class Function
    : public Statement
{
private:
    // TO-DO: specify data type later
    Variable name; // function name
    std::vector<ExpressionPtr> arg; // multiple arguments
    std::vector<StatementPtr> statements;
protected:
    Function(Variable _name, std::vector<ExpressionPtr> _arg = {}, std::vector<StatementPtr> _statements = {})
        : name(_name)
        , arg(_arg) 
        , statements(_statements)
    {}
public:
    virtual ~Function()
    {
        for(int i = 0; i < arg.size(); i++) {
            delete arg[i];
        }
    }

    virtual const char *getFunction() const =0;

    std::vector<ExpressionPtr> getArg() const
    { return arg; }

    virtual void print(std::ostream &dst) const override
    {
        name.print(dst);
        dst<<"( ";
        for(int i = 0; i < arg.size(); i++) {
            arg[i]->print(dst);
        }
        dst<<" )";
    }

};

#endif