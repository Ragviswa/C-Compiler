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
public:
    Function(Variable *_name, StatementPtr _statements = nullptr)
        : name(_name)
        , statements(_statements)
    {}

    Function(Variable *_name, ExpressionPtr _arg = nullptr, StatementPtr _statements = nullptr)
        : name(_name)
        , arg(_arg) 
        , statements(_statements)
    {}

    virtual ~Function()
    {
        delete name,
        delete arg,
        delete statements;
    }

    const std::string getFunction() const
    {
        return name->getId();
    }

    ExpressionPtr getArg() const
    { return arg; }

    void CompileRec(std::string destReg) const {
        std::cout << ":" << name->getId() << std::endl;
        statements->CompileRec(destReg);
    }

    void print(std::ostream &dst) const
    {
        name->print(dst);
        dst<<"( ";
        dst<<" )";
        dst<<'\n';
        statements->print(dst);
    }

};
#endif