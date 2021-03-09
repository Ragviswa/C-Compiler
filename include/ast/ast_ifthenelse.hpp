#ifndef ast_loops_hpp
#define ast_loops_hpp

#include <vector>

class IfThenElse
    : public Statement
{
private:
    ExpressionPtr condition;
    std::vector<StatementPtr> if_branch;
    std::vector<StatementPtr> else_branch;
protected:
    Loop(ExpressionPtr _condition, std::vector<StatementPtr> _if_branch = {}, std::vector<StatementPtr> _else_branch = {})
        : condition(_condition)
        , if_branch(_statements)
        , else_branch(_else_branch)
    {}
public:
    virtual ~IfThenElse()
    {
        delete condition;
        delete if_branch;
        delete else_branch;
    }

    virtual const char *getOpcode() const =0;

    ExpressionPtr getCondition() const 
    { return condition; }

    std::vector<StatementPtr> getIfBranch() const
    { return if_branch; }

    std::vector<StatementPtr> getElseBranch() const
    { return else_branch; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<getOpcode();
        dst<<" ( ";
        condition->print(dst);
        dst<<" ) ";
        dst<<"{ ";
        for(int i = 0; i < if_branch.size(); i++) {
            if_branch[i]->print(dst);
        }
        dst<<" }";
        dst<<"else"
        dst<<"{ ";
        for(int i = 0; i < else_branch.size(); i++) {
            else_branch[i]->print(dst);
        }
        dst<<" }";
    }
};

class IfThenElseStatement
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