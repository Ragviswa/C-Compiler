#ifndef ast_statement_hpp
#define ast_statement_hpp

#include "ast_expression.hpp"

class Statement;

typedef const Statement *StatementPtr;

class Statement
    : public Expression
{
public:
    virtual ~Statement() {}

    virtual void print(std::ostream &dst) const =0;
};

#endif