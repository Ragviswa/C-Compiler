#ifndef ast_unary_hpp
#define ast_unary_hpp

#include <string>
#include <iostream>

class Unary
    : public Expression
{
private:
    ExpressionPtr expr;
protected:
    Unary(const ExpressionPtr _expr)
        : expr(_expr)
    {}
public:
    virtual ~Unary()
    {
        delete expr;
    }

    virtual const char *getOpcode() const =0;

    virtual const std::string getDataType() const override {
        return expr->getDataType();
    }

    ExpressionPtr getExpr() const
    { return expr; }

    virtual void print(std::ostream &dst) const override
    {
        dst << "( ";
        dst << getOpcode();
        dst << " ";
        expr->print(dst);
        dst << " )";
    }
};

class NegOperator
    : public Unary
{
public:
    NegOperator(const ExpressionPtr _expr)
        : Unary(_expr)
    {}

    virtual const char *getOpcode() const override
    { return "-"; }

    virtual int evaluate() const override
    {
        int vl=getExpr()->evaluate();
        return -vl;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string type = getExpr()->getDataType();
        if(type == "INT") {
            getExpr()->CompileRec("$t0");
            std::cout << "subu " << destReg << ", $0, $t0" << std::endl;
        }
        else if(type == "FLOAT") {
            getExpr()->CompileRec("$f0");
            std::cout << "sub.s " << destReg << ", $0, $f0" << std::endl;
        }
        else if(type == "DOUBLE") {
            getExpr()->CompileRec("$f0");
            std::cout << "sub.d " << destReg << ", $0, $f0" << std::endl;
        }
        else if(type == "CHAR") {
            getExpr()->CompileRec("$t0");
            std::cout << "sub " << destReg << ", $0, $f0" << std::endl;
            std::cout << "andi " << destReg << ", " << destReg << ", 0x00ff" << std::endl;
        }
    }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override
    {
        double in=getExpr()->evaluate(bindings);
        return (-in);
    }
};

class PosOperator
    : public Unary
{
public:
    PosOperator(const ExpressionPtr _expr)
        : Unary(_expr)
    {}

    virtual const char *getOpcode() const override
    { return "+"; }

    virtual int evaluate() const override
    {
        int vl=getExpr()->evaluate();
        return +vl;
    }

    virtual void CompileRec(std::string destReg) const override {
        std::string type = getExpr()->getDataType();
        if(type == "INT") {
            getExpr()->CompileRec("$t0");
            std::cout << "subu " << destReg << ", $t0, $0" << std::endl;
        }
        else if(type == "FLOAT") {
            getExpr()->CompileRec("$f0");
            std::cout << "sub.s " << destReg << ", $f0, $0" << std::endl;
        }
        else if(type == "DOUBLE") {
            getExpr()->CompileRec("$f0");
            std::cout << "sub.d " << destReg << ", $f0, $0" << std::endl;
        }
        else if(type == "CHAR") {
            getExpr()->CompileRec("$t0");
            std::cout << "sub " << destReg << ", $t0, $0" << std::endl;
            std::cout << "andi " << destReg << ", " << destReg << ", 0x00ff" << std::endl;
        }
    }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override
    {
        double in=getExpr()->evaluate(bindings);
        return (in);
    }
};

class NotLogic
    : public Unary
{
public:
    NotLogic(const ExpressionPtr _expr)
        : Unary(_expr)
    {}

    virtual const char *getOpcode() const override
    { return "!"; }

    virtual int evaluate() const override
    {
        int vl=getExpr()->evaluate();
        return !vl;
    }
    
    virtual void CompileRec(std::string destReg) const override {
        getExpr()->CompileRec("$t0");
        std::string set_zero = makeName("set_zero");
        std::cout << "beq $t0, $0, " << set_zero << std::endl;
        std::cout << "addi " << destReg << ", $0, 1" << std::endl;
        std::cout << ":" << set_zero << std::endl;
        std::cout << "add " << destReg << ", $0, $0" << std::endl;
    }

    virtual double evaluate(
        const std::map<std::string, double> &bindings
    ) const override
    {
        double in=getExpr()->evaluate(bindings);
        return (!in);
    }
};

#endif
