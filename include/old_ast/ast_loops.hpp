#ifndef ast_loops_hpp
#define ast_loops_hpp

#include <vector>

class Loop 
    : public Statement
{
private:
    ExpressionPtr condition;
    std::vector<StatementPtr> statements;
protected:
    Loop(ExpressionPtr _condition, std::vector<StatementPtr> _statements)
        : condition(_condition)
        , statements(_statements)
    {}
public:
    virtual ~Loop()
    {
        delete condition;
        delete statements;
    }

    virtual const char *getOpcode() const =0;

    ExpressionPtr getCondition() const 
    { return condition; }

    std::vector<StatementPtr> getStatements() const
    { return statements; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<getOpcode();
        dst<<" ( ";
        condition->print(dst);
        dst<<" ) ";
        dst<<"{ ";
        for(int i = 0; i < statements.size(); i++) {
            statements[i]->print(dst);
        }
        dst<<" }";
    }
};

class WhileLoop
    : public Loop
{
protected:
    virtual const char *getOpcode() const override
    { return "while"; }
public:
    WhileLoop(ExpressionPtr _condition, std::vector<StatementPtr> _statements)
        : Loop(_condition, _statements)
    {}

};

class IfStatement
    : public Loop
{
protected:
    virtual const char *getOpcode() const override
    { return "if"; }
public:
    WhileLoop(ExpressionPtr _condition, std::vector<StatementPtr> _statements)
        : Loop(_condition, _statements)
    {}

};

#endif