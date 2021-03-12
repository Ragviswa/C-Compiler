#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

enum TypeDef{
  INT,
  FLT,
  DBL
};

class Variable
    :public Expression
{
private:
    std::string type;
    std::string id;
    ExpressionPtr Expr;
public:
//This constructor does not feel right, it's a placeholder.
    Variable() {
    }
    
    Variable(const std::string *_id) {
        id = *_id;
    }

    Variable(TypeDef _type, const std::string *_id, ExpressionPtr _Expr = nullptr) {
        switch(_type) {
            case INT:
                type = "INT";
                id = *_id;
                Expr = _Expr;
                break;
            case FLT:
                type = "FLOAT";
                id = *_id;
                Expr = _Expr;
                break;
            case DBL:
                type = "DOUBLE";
                id = *_id;
                Expr = _Expr;
            default:
                type = "something went wrong";
        }
    }
    
    const std::string getType() const
    { return type; }

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<type;
        dst<<" ";
        dst<<id;
        if(Expr!=nullptr){
            dst<<" = ";
            Expr->print(dst);
        }
        dst<<";";
        dst<<'\n';
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return bindings.at(id);
    }    
};

class DeclarationList;

typedef const DeclarationList *DeclarationListPtr;

class DeclarationList
    : public Variable
{
private:
    Variable *variable;
    DeclarationListPtr declarationList;
public:
    DeclarationList(Variable *_variable, DeclarationListPtr _declarationList = nullptr)
        : variable(_variable)
        , declarationList(_declarationList)
    {}

    virtual ~DeclarationList() {
        delete variable;
        delete declarationList;
    }

    virtual void print(std::ostream &dst) const override
    {
        variable->print(dst);
        if(declarationList!=nullptr){
            declarationList->print(dst);
        }
    }
};

class Number
    : public Expression
{
private:
    double value;
public:
    Number(double _value)
        : value(_value)
    {}

    double getValue() const
    { return value; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<value;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return value;
    }
};

#endif
