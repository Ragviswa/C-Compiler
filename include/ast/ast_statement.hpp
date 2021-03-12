#ifndef ast_statement_hpp
#define ast_statement_hpp

#include "ast_expression.hpp"
#include "ast_primitives.hpp"

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

    virtual void print(std::ostream &dst) const override
    {
        statement->print(dst);
        if(statementList!=nullptr){
            statementList->print(dst);
        }
    }
};

class SelectStatement
    : public Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement;
public:
    SelectStatement(ExpressionPtr _condition, StatementPtr _statement = nullptr)
        : condition(_condition)
        , statement(_statement)
    {}
    virtual ~SelectStatement() {
        delete condition;
        delete statement;
    }
    ExpressionPtr getCond() const
    { return condition; }
    StatementPtr getStat() const
    { return statement; }
    
};

class IfStatement
    : public SelectStatement
{
private:
    StatementPtr else_branch;
public:
    IfStatement(ExpressionPtr _condition, StatementPtr _if_branch = nullptr, StatementPtr _else_branch = nullptr)
        : SelectStatement(_condition, _if_branch)
        , else_branch(_else_branch)
    {}
    ~IfStatement() {
        delete else_branch;
    }

    virtual void print(std::ostream &dst) const override
    {
        dst<<"if ( ";
        getCond()->print(dst);
        dst<<" ) ";
        getStat()->print(dst);
        if(else_branch!=nullptr) {
            dst<<"else ";
            else_branch->print(dst);
        }
        dst<<'\n';
    }
};

class LoopStatement
    : public Statement
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
    ExpressionPtr getCond() const
    { return condition; }
    StatementPtr getStat() const
    { return statement; }
};

class WhileLoop
    : public LoopStatement
{
public:
    WhileLoop(ExpressionPtr _condition, StatementPtr _statement = nullptr)
        : LoopStatement(_condition, _statement)
    {}

    virtual void print(std::ostream &dst) const override
    {
        dst<<"while ( ";
        getCond()->print(dst);
        dst<<" ) ";
        getStat()->print(dst);
        dst<<'\n';
    }
};

class ExpressionStatement
    : public Statement
{
private:
    ExpressionPtr expression;
public:
    ExpressionStatement(ExpressionPtr _expression = nullptr)
        : expression(_expression)
    {}
    ~ExpressionStatement() {
        delete expression;
    }
    virtual void print(std::ostream &dst) const override
    {
        if(expression!=nullptr){
            expression->print(dst);
        }
        dst<<";";
        dst<<'\n';
    }
};

class JumpStatement
    : public Statement
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
    virtual void print(std::ostream &dst) const override
    {
        dst<<"return ";
        if(expression!=nullptr){
            expression->print(dst);
        }
        dst<<";";
        dst<<'\n';
    }
};

class CompoundStatement
    : public Statement
{
private:
    StatementListPtr statementList;
    DeclarationListPtr declarationList;
public:
     CompoundStatement()
    {} 
    CompoundStatement(StatementListPtr _statementList)
        : statementList(_statementList)
    {} 
    CompoundStatement(DeclarationListPtr _declarationList)
        : declarationList(_declarationList)
    {} 
    CompoundStatement(DeclarationListPtr _declarationList, StatementListPtr _statementList)
        : statementList(_statementList)
        , declarationList(_declarationList)
    {} 
    ~CompoundStatement() {
        delete statementList;
        delete declarationList;
    }
    virtual void print(std::ostream &dst) const override
    {
        dst<<"{ ";
        if(declarationList!=nullptr){
            declarationList->print(dst);
        }
        if(statementList!=nullptr){
            statementList->print(dst);
        }
        dst<<"}";
        dst<<'\n';
    }
};

#endif