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

    virtual void CompileRec(std::string destReg) const =0;
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
    StatementPtr getStat() const
    { return statement; }
    StatementListPtr getstatlist() const
    { return statementList; }
    virtual void print(std::ostream &dst) const override
    {
        statement->print(dst);
        if(statementList!=nullptr){
            statementList->print(dst);
        }
    }
    virtual void CompileRec(std::string destReg) const override{
        getStat()->CompileRec(destReg);
        if(statementList!=nullptr){
            getstatlist()->CompileRec(destReg);
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
    StatementPtr getElse() const
    { return else_branch; }


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

    virtual void CompileRec(std::string destReg) const override {
        std::string c = makeName("c");
        getCond()->CompileRec(c);
        std::string exit = makeName("exit");
        if(else_branch!=nullptr){
            std::string else_stat = makeName("else_stat");
            std::cout << "beq " << c << " $0 " << else_stat <<std::endl;
            getStat()->CompileRec(destReg);
            std::string exit = makeName("exit");
            std::cout << "jump " << exit << std::endl;
            std::cout << ":" << else_stat << std::endl;
            getElse()->CompileRec(destReg);
            std::cout << ":"<<exit<<std::endl;
        }else{
            std::string exit = makeName("exit");
            std::cout << "beq " << c << " $0 " << exit <<std::endl;
            getStat()->CompileRec(destReg);
            std::cout << ":"<<exit<<std::endl;
        }
    }
};

class SwitchStatement
    : public SelectStatement
{
public:
    SwitchStatement(ExpressionPtr _condition, StatementPtr _stat)
        : SelectStatement(_condition, _stat)
    {}
    ~SwitchStatement() {
    }

    virtual void print(std::ostream &dst) const override
    {
        dst<<"switch ( ";
        getCond()->print(dst);
        dst<<" ) ";
        getStat()->print(dst);
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override {
        //needs implementation
    }
};

class LoopStatement
    : public Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement;
public:
    LoopStatement() {}
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

    virtual void CompileRec(std::string destReg) const override{
        std::string c = makeName("while_condition");
        getCond()->CompileRec(c);
        std::string unique_exit = makeName("exit");
        std::cout << "beq " << c << " $0 " << unique_exit << std::endl;
        std::string unique_start = makeName("start");
        std::cout << ":" << unique_start << std::endl;
        getStat()->CompileRec(destReg);
        getCond()->CompileRec(c);
        std::cout << "bne " << c << " $0 " << unique_start << std::endl;
        std::cout << ":" << unique_exit << std::endl;
        std::cout << "add " << destReg << " $0 $0" << std::endl;
    }
};

class ForLoop
    : public LoopStatement
{
private:
    Variable *initVar = nullptr; // int x = 0
    ExpressionPtr initExpr = nullptr; // x = 0
    //ExpressionPtr testExpr;
    ExpressionPtr updateExpr;
    //StatementPtr statement;
public:
    ForLoop(ExpressionPtr _initExpr, ExpressionPtr _testExpr, ExpressionPtr _updateExpr, StatementPtr _statement)
     : LoopStatement(_testExpr, _statement)
     , initExpr(_initExpr)
     , updateExpr(_updateExpr)
    {}
    ForLoop(Variable *_initVar, ExpressionPtr _testExpr, ExpressionPtr _updateExpr, StatementPtr _statement)
    : LoopStatement(_testExpr, _statement)
    , initVar(_initVar)
    , updateExpr(_updateExpr)
    {}
    ~ForLoop() {
        delete initVar;
        delete initExpr;
        delete updateExpr;
    }
    virtual void print(std::ostream &dst) const override
    {
        dst<<"for ( ";
        if(initVar != nullptr && initExpr == nullptr) {
            initVar->print(dst);
        }
        else if(initVar == nullptr && initExpr != nullptr) {
            initExpr->print(dst);
            dst<<" ; ";
        }
        getCond()->print(dst);
        dst<<" ; ";
        updateExpr->print(dst);
        dst<<" ) \n";
        getStat()->print(dst);
        dst<<'\n';
    }
    virtual void CompileRec(std::string destReg) const override{
        //needs implementation
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
    ExpressionPtr getExp() const
    { return expression; }
    virtual void print(std::ostream &dst) const override
    {
        if(expression!=nullptr){
            expression->print(dst);
        }
        dst<<";";
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override {
        getExp()->CompileRec(destReg);
    }
};

class JumpStatement
    : public Statement
{
public:
    JumpStatement()
    {}
    ~JumpStatement() 
    {}
};

class ReturnStatement
    : public JumpStatement
{
private:
    ExpressionPtr expression;
public:
    ReturnStatement(ExpressionPtr _expression = nullptr)
        : expression(_expression)
    {}
    ~ReturnStatement() {
        delete expression;
    }
    ExpressionPtr getExp() const
    { return expression; }
    virtual void print(std::ostream &dst) const override
    {
        dst<<"return ";
        if(expression!=nullptr){
            expression->print(dst);
        }
        dst<<";";
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override{
        std::string exp = makeName("exp");
        getExp()->CompileRec(exp);
        std::cout << "add $2 $0 " << exp << std::endl;
    }
};

class ContinueStatement
    : public JumpStatement
{
public:
    ContinueStatement()
    {}
    ~ContinueStatement() 
    {}

    virtual void print(std::ostream &dst) const override
    {
        dst<<"continue;";
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override{
        //needs implementation
    }
};

class BreakStatement
    : public JumpStatement
{
public:
    BreakStatement()
    {}
    ~BreakStatement() 
    {}

    virtual void print(std::ostream &dst) const override
    {
        dst<<"break;";
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override{
        //needs implementation
    }
};

class LabelStatement
    : public Statement
{
private:
    ExpressionPtr expression;
    StatementPtr statement;
public:
    LabelStatement(ExpressionPtr _expression = nullptr, StatementPtr _statement = nullptr)
        : expression(_expression)
        , statement(_statement)
    {}
    ~LabelStatement() {
        delete expression;
        delete statement;
    }
    ExpressionPtr getExp() const
    { return expression; }
    StatementPtr getStat() const
    { return statement; }
    virtual void print(std::ostream &dst) const override
    {
        dst<<"case ";
        if(expression!=nullptr){
            expression->print(dst);
        }
        dst<<": ";
        if(statement!=nullptr){
            statement->print(dst);
        }
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override {
        //needs implementation
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
    StatementListPtr getstatlist() const
    { return statementList; }
    DeclarationListPtr getdecllist() const
    { return declarationList; }
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

    virtual void CompileRec(std::string destReg) const override{
        if(declarationList!=nullptr){
            declarationList->CompileRec(destReg);
        }
        if(statementList!=nullptr){
            statementList->CompileRec(destReg);
        }
    }    
};

#endif