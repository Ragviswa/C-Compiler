#ifndef ast_loops_hpp
#define ast_loops_hpp

class Loop 
    : public Statement
{
private:
    ExpressionPtr condition;
    StatementPtr statement;
protected:
    Loop(ExpressionPtr _condition, StatementPtr _statement)
        : condition(_condition)
        , statement(_statement)
    {}
public:
    virtual ~Loop()
    {
        delete condition;
        delete statement;
    }

    virtual const char *getOpcode() const =0;

    ExpressionPtr getCondition() const 
    { return condition; }

    StatementPtr getStatement() const
    { return statement; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<getOpcode();
        dst<<" ( ";
        condition->print(dst);
        dst<<" ) ";
        dst<<"{ ";
        statement->print(dst);
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
    WhileLoop(ExpressionPtr _condition, StatementPtr _statement)
        : Loop(_condition, _statement)
    {}
    
};

#endif