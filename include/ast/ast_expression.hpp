#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <string>
#include <iostream>
#include <map>

#include <memory>
#include "symbol_table.hpp"

extern SymbolTable Symbol;
extern StackPtr StackPointer;

class Expression;

typedef const Expression *ExpressionPtr;

class Expression
{
public:
    virtual ~Expression()
    {}

    //! Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst) const
    {}

    virtual const std::string getDataType() const
    {}

    virtual void CompileRec(std::string destReg) const
    {}

    //! Evaluate the tree using the given mapping of variables to numbers
    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const
    { throw std::runtime_error("Not implemented."); }
};

class ExpressionList;

typedef const ExpressionList *ExpressionListPtr;

class ExpressionList
    : public Expression
{
private:
    ExpressionPtr arg = nullptr;
    ExpressionListPtr arg_list = nullptr;
public:
    ExpressionList(ExpressionPtr _arg, ExpressionListPtr _arg_list = nullptr)
        : arg(_arg)
        , arg_list(_arg_list)
    {}

    virtual ~ExpressionList() {
        delete arg;
        delete arg_list;
    }
    ExpressionPtr getArg() const
    { return arg; }

    ExpressionListPtr getArgList() const
    { return arg_list;}

    virtual void CompileRec(std::string destReg, std::string address) const{
        if(getArg() != nullptr){
            if(StackPointer.getArgc()<4){
                StackPointer.setIncr(StackPointer.getIncr()+4);
                StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                std::cout << "addiu $sp, $sp, -4" << std::endl; 
                getArg()->CompileRec("$a" + std::to_string(StackPointer.getArgc()));
            }else{
                getArg()->CompileRec("$t0");
                StackPointer.setIncr(StackPointer.getIncr()+4);
                StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                std::cout << "addiu $sp, $sp, -4" << std::endl;
                std::cout << "sw $t0, " << std::to_string(StackPointer.getArgc()*4) << "($sp)"<< std::endl;
            }
            StackPointer.setArgc(StackPointer.getArgc()+1);
        }
        if(getArgList()!=nullptr){
            getArgList()->CompileRec(destReg, address);
        }
    }
};

static int makeNameUnq = 0;
static std::string makeName(std::string base) {
    return "_" + base + "_" + std::to_string(makeNameUnq++);
}


#endif
