#ifndef ast_statement_hpp
#define ast_statement_hpp

#include "ast_expression.hpp"

class Statement;

typedef const Statement *StatementPtr;

class Statement
{
public:
    virtual ~Statement() {}

    virtual void print(std::ostream &dst) const =0;
};

class StatementList;

typedef const StatementList *StatementListPtr;

class StatementList
    : public Statement
{
private:
    StatementPtr statement;
    StatementListPtr statementList;
public:
    StatementList(StatementPtr _statement, StatementListPtr _statementList = nullptr)
        : statement(_statement)
        , statementList(_statementList)
    {}
    virtual ~StatementList() {
        delete statement;
        delete statementList;
    }
};

class SelectStatement
    : public Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement;
public:
    SelectStatement(ExpressionPtr _condition, StatementPtr _statement)
        : condition(_condition)
        , statement(_statement)
    {}
    virtual ~SelectStatement() {
        delete condition;
        delete statement;
    }
};

class IfStatement
    : SelectStatement
{
private:
    StatementPtr else_branch;
public:
    IfStatement(ExpressionPtr _condition, StatementPtr _if_branch, StatementPtr _else_branch = nullptr)
        : SelectStatement(_condition, _if_branch)
        , else_branch(_else_branch)
    {}
    ~IfStatement() {
        delete else_branch;
    }
};

class LoopStatement
    : Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement;
public:
    LoopStatement(ExpressionPtr _condition, StatementPtr _statement = nullptr)
        : condition(_condition)
        , statement(_statement)
    {}
    ~LoopStatement() {
        delete condition;
        delete statement;
    }
};

class WhileLoop
    : LoopStatement
{
public:
    WhileLoop(ExpressionPtr _condition, StatementPtr _statement = nullptr)
        : LoopStatement(_condition, _statement)
    {}
};

class ExpressionStatement
    : Statement
{
private:
    ExpressionPtr expression;
public:
    ExpressionStatement(ExpressionPtr _expression)
        : expression(_expression)
    {}
    ~ExpressionStatement() {
        delete expression;
    }
};

class JumpStatement
    : Statement
{
private:
    ExpressionPtr expression;
public:
    JumpStatement(ExpressionPtr _expression = nullptr)
        : expression(_expression)
    {}
    ~JumpStatement() {
        delete expression;
    }
};

class CompoundStatement
    : Statement
{
private:
    StatementListPtr statementList;
public:
    CompoundStatement(StatementListPtr _statementList = nullptr)
        : statementList(_statementList)
    {} 
    ~CompoundStatement() {
        delete statementList;
    }
};

#endif