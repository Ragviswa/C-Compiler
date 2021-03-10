#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

class Variable
    :Expression
{
private:
    std::string type;
    std::string id;
public:
    Variable(const std::string*_type, const std::string *_id)
        : type(*_type)
        , id(*_id)
    {}

    const std::string getType() const
    { return type; }

    const std::string getId() const
    { return id; }

    virtual void print(std::ostream &dst) const override
    {
        dst<<id;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return bindings.at(id);
    }    
};

class Integer
    :Variable
{
private:
    int value;
public:
    Integer(const std::string*_type, const std::string *_id, int _value = 0)
        : Variable(_type, _id)
        , value(_value)
    {}

    const int getValue() const
    { return value; }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override
    {
        return bindings.at(id);
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
