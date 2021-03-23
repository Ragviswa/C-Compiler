#ifndef ast_statement_hpp
#define ast_statement_hpp

#include "ast_expression.hpp"
#include "ast_primitives.hpp"
#include "symbol_table.hpp"


class Statement;

typedef const Statement *StatementPtr;

class Statement
{
public:
    virtual ~Statement() {}

    virtual void print(std::ostream &dst) const =0;

    virtual void CompileRec(std::string destReg) const =0;
};

class BlockList;

typedef const BlockList *BlockListPtr;

class BlockList
    : public Statement
{
private:
    StatementPtr statement = nullptr;
    Variable *variable = nullptr;
    BlockListPtr statdecllist = nullptr;
public:
    BlockList(StatementPtr _statement, BlockListPtr _statdecllist = nullptr)
        : statement(_statement)
        , statdecllist(_statdecllist)
    {}
    BlockList(Variable *_variable, BlockListPtr _statdecllist = nullptr)
        : variable(_variable)
        , statdecllist(_statdecllist)
    {}
    virtual ~BlockList() {
        delete statement;
        delete statdecllist;
        delete variable;
    }
    StatementPtr getStat() const
    { return statement; }
    Variable *getVar() const
    { return variable;}
    BlockListPtr getstatdecllist() const
    { return statdecllist; }

    virtual void print(std::ostream &dst) const override
    {
        if(statement != nullptr){
            statement->print(dst);
        }else if(variable != nullptr){
            variable->print(dst);
        }
        if(statdecllist!=nullptr){
            statdecllist->print(dst);
        }
    }
    virtual void CompileRec(std::string destReg) const override{
        if(getStat() != nullptr){
            getStat()->CompileRec(destReg);
        }else if(getVar() != nullptr){
            getVar()->CompileRec(destReg);
        }
        if(getstatdecllist()!=nullptr){
            getstatdecllist()->CompileRec(destReg);
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
    StatementPtr else_branch = nullptr;
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
        dst<<'\n';
        getStat()->print(dst);
        if(else_branch!=nullptr) {
            dst<<"else ";
            else_branch->print(dst);
        }
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override {
        getCond()->CompileRec("$t0");
        std::string exit = makeName("exit");
        if(else_branch!=nullptr){
            std::string else_stat = makeName("else_stat");
            std::cout << "beq $t0, $0, " << else_stat << std::endl;
            getStat()->CompileRec(destReg);
            std::string exit = makeName("exit");
            std::cout << "j " << exit << std::endl;
            std::cout << else_stat << ":" << std::endl;
            getElse()->CompileRec(destReg);
            std::cout << exit << ":" << std::endl;
        }else{
            std::string exit = makeName("exit");
            std::cout << "beq $t0, $0, " << exit << std::endl;
            getStat()->CompileRec(destReg);
            std::cout << exit << ":" << std::endl;
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
        getCond()->CompileRec("$t0");
        std::string exit = makeName("exit");
        std::cout << "beq $t0, $0, " << exit << std::endl;
        getStat()->CompileRec(destReg);
        std::cout << exit << ":" << std::endl;
    }
};

class LoopStatement
    : public Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement = nullptr;
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
        std::string unique_exit = makeName("exit");
        std::string unique_loop = makeName("loop");
        Symbol.setloopstart(unique_loop);
        Symbol.setloopend(unique_exit);
        std::cout << unique_loop << ":" << std::endl;
        getCond()->CompileRec("$t0");
        std::cout << "beq $t0, $0, " << unique_exit << std::endl;
        getStat()->CompileRec(destReg); // loop body
        std::cout << "j " << unique_loop << std::endl;
        std::cout << unique_exit << ":" << std::endl;
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
        if(initVar != nullptr && initExpr == nullptr) { // int i = 0;
            initVar->getExpr()->CompileRec("$t0");
        }
        else if(initVar == nullptr && initExpr != nullptr) { // i = 0;
            initExpr->CompileRec("$t0");
        }
        else {
            std::cout << "ERROR: no initial value in for loop" << std::endl;
        }
        
        std::string unique_loop = makeName("loop");
        std::cout << unique_loop << ":" << std::endl;
        std::string unique_exit = makeName("exit");
        Symbol.setloopstart(unique_loop);
        Symbol.setloopend(unique_exit);
        getCond()->CompileRec("$t1"); // i < 3
        std::cout << "beq $t1, $0, " << unique_exit << std::endl; // exit if condition false
        getStat()->CompileRec(destReg); // loop body
        updateExpr->CompileRec("$t0");
        std::cout << "j " << unique_loop << std::endl;
        std::cout << unique_exit << ":" << std::endl;
    }
};

class ExpressionStatement
    : public Statement
{
private:
    ExpressionPtr expression = nullptr;
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
    ExpressionPtr expression = nullptr;
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
        if(destReg[1] == 'f') {
            getExp()->CompileRec("$f0");
            std::cout << "j " << Symbol.getfuncend() << std::endl;
        }else{
            getExp()->CompileRec("$t0");
            if(StackPointer.getNullfunc()==1){
                StackPointer.setNullfunc(0);
            }else{
                std::cout << "add $v0, $0, $t0" << std::endl;
                std::cout << "j " << Symbol.getfuncend() << std::endl;
            }
        }
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
        std::cout << "j " << Symbol.getloopstart() << std::endl;
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
        std::cout << "j " << Symbol.getloopend() << std::endl;
    }
};

class LabelStatement
    : public Statement
{
private:
    ExpressionPtr expression = nullptr;
    StatementPtr statement = nullptr;
public:
    LabelStatement(StatementPtr _statement, ExpressionPtr _expression = nullptr) 
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
    BlockListPtr blocklist = nullptr;
public:
    CompoundStatement()
    {} 
    CompoundStatement(BlockListPtr _blocklist)
        : blocklist(_blocklist)
    {}
    ~CompoundStatement() {
        delete blocklist;
    }
    BlockListPtr getblocklist() const
    { return blocklist; }
    virtual void print(std::ostream &dst) const override
    {
        dst<<"{ ";
        if(blocklist!=nullptr){
            blocklist->print(dst);
        }
        dst<<"}";
        dst<<'\n';
    }

    virtual void CompileRec(std::string destReg) const override{
        Symbol.newScope();
        StackPointer.setcurrentscope(StackPointer.getcurrentscope()+1);
        StackPointer.setscopeIncr(0);
        if(getblocklist()!=nullptr){
            getblocklist()->CompileRec(destReg);
        }
        std::cout << "addiu $sp, $sp, " << StackPointer.getscopeIncr() << std::endl;
        StackPointer.setIncr(StackPointer.getIncr() - StackPointer.getscopeIncr());
        StackPointer.setcurrentscope(StackPointer.getcurrentscope()-1);
        Symbol.endScope();
    }
};

#endif