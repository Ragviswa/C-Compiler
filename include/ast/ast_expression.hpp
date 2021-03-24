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

    virtual const std::string getDataFormat() const
    { return "var"; }

    virtual const int getDataLength() const
    {}

    virtual void CompileRec(std::string destReg) const
    {}

    virtual int evaluate() const
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
                std::string argType = getArg()->getDataType();
                std::string argFormat = getArg()->getDataFormat();
                if(argFormat == "ptr" || argFormat == "array") {
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    std::cout << "addiu $sp, $sp, -4" << std::endl;
                    getArg()->CompileRec("$a" + std::to_string(StackPointer.getArgc()));
                    StackPointer.setArgSize(StackPointer.getArgSize()+4);
                } else if(argFormat == "var") {
                    if(argType == "INT") {
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -4" << std::endl;
                        getArg()->CompileRec("$a" + std::to_string(StackPointer.getArgc()));
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                    else if(argType == "FLOAT") {
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -4" << std::endl;
                        std::string floatArgReg;
                        switch(StackPointer.getArgc()) {
                            case 0:
                                floatArgReg = "$f12";
                                break;
                            case 1:
                                floatArgReg = "$f14";
                                break;
                            case 2:
                                floatArgReg = "$6";
                                break;
                            case 3:
                                floatArgReg = "$7";
                                break;
                        }
                        getArg()->CompileRec(floatArgReg);
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                    else if(argType == "DOUBLE") {
                        StackPointer.setIncr(StackPointer.getIncr()+8);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                        std::cout << "addiu $sp, $sp, -8" << std::endl;
                        std::string floatArgReg;
                        switch(StackPointer.getArgc()) {
                            case 0:
                                floatArgReg = "$f12";
                                break;
                            case 1:
                                floatArgReg = "$f14";
                                break;
                            case 2:
                                floatArgReg = "$f16";
                                break;
                            case 3:
                                floatArgReg = "$f18";
                                break;
                        }
                        getArg()->CompileRec(floatArgReg);
                        StackPointer.setArgSize(StackPointer.getArgSize()+8);
                    }
                    else if(argType == "CHAR") {
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -" << 4 << std::endl;
                        getArg()->CompileRec("$a" + std::to_string(StackPointer.getArgc()));
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                }    
            }else{
                std::string argType = getArg()->getDataType();
                std::string argFormat = getArg()->getDataFormat();
                if(argFormat == "ptr" || argFormat == "array") {
                    getArg()->CompileRec("$t0");
                    StackPointer.setIncr(StackPointer.getIncr()+4);
                    StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                    std::cout << "addiu $sp, $sp, -4" << std::endl;
                    std::cout << "sw $t0, " << std::to_string(StackPointer.getArgSize()) << "($sp)" << std::endl;
                    StackPointer.setArgSize(StackPointer.getArgSize()+4);
                } else if(argFormat == "var") {
                    if(argType == "INT") {
                        getArg()->CompileRec("$t0");
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -4" << std::endl;
                        std::cout << "sw $t0, " << std::to_string(StackPointer.getArgSize()) << "($sp)" << std::endl;
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                    else if(argType == "FLOAT") {
                        getArg()->CompileRec("$f0");
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -4" << std::endl;
                        std::cout << "swc1 $f0, " << std::to_string(StackPointer.getArgSize()) << "($sp)" << std::endl;
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                    else if(argType == "DOUBLE") {
                        getArg()->CompileRec("$f0");
                        StackPointer.setIncr(StackPointer.getIncr()+8);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+8);
                        std::cout << "addiu $sp, $sp, -8" << std::endl;
                        std::cout << "swc1 $f0, " << std::to_string(StackPointer.getArgSize()) << "($sp)" << std::endl;
                        std::cout << "swc1 $f1, " << std::to_string(StackPointer.getArgSize()+4) << "($sp)" << std::endl;
                        StackPointer.setArgSize(StackPointer.getArgSize()+8);
                    }
                    else if(argType == "CHAR") {
                        getArg()->CompileRec("$t0");
                        StackPointer.setIncr(StackPointer.getIncr()+4);
                        StackPointer.setscopeIncr(StackPointer.getscopeIncr()+4);
                        std::cout << "addiu $sp, $sp, -" << 4 << std::endl;
                        std::cout << "sw $t0, " << std::to_string(StackPointer.getArgSize()) << "($sp)" << std::endl;
                        StackPointer.setArgSize(StackPointer.getArgSize()+4);
                    }
                }
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
