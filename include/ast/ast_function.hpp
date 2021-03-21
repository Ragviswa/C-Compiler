#ifndef ast_function_hpp
#define ast_function_hpp

#include "ast_primitives.hpp"
#include "ast_statement.hpp"
#include "ast_expression.hpp"

class Function
{
private:
    
    Variable *name; // function name
    DeclarationListPtr args;
    StatementPtr statements;
public:
    Function(Variable *_name, StatementPtr _statements = nullptr)
        : name(_name)
        , statements(_statements)
    {}

    Function(Variable *_name, DeclarationListPtr _args = nullptr, StatementPtr _statements = nullptr)
        : name(_name)
        , args(_args) 
        , statements(_statements)
    {}

    virtual ~Function()
    {
        delete name,
        delete args,
        delete statements;
    }

    const std::string getFunction() const
    {
        return name->getId();
    }

    DeclarationListPtr getArgs() const
    { return args; }

    void CompileRec(std::string destReg) const {
        std::cout << name->getId() << ":" << std::endl;
        std::cout << "move $fp, $sp" << std::endl;
        std::cout << "addiu $sp, $sp, -44" << std::endl;
        std::cout << "sw $s0, 4($sp)" << std::endl;
        std::cout << "sw $s1, 8($sp)" << std::endl;
        std::cout << "sw $s2, 12($sp)" << std::endl;
        std::cout << "sw $s3, 16($sp)" << std::endl;
        std::cout << "sw $s4, 20($sp)" << std::endl;
        std::cout << "sw $s5, 24($sp)" << std::endl;
        std::cout << "sw $s6, 28($sp)" << std::endl;
        std::cout << "sw $s7, 32($sp)" << std::endl;
        std::cout << "sw $fp, 36($sp)" << std::endl;
        std::cout << "sw $ra, 40($sp)" << std::endl;
        StackPointer.setIncr(StackPointer.getIncr()+44);

        if(args!=nullptr){
            Symbol.newScope();
            StackPointer.setcurrentscope(StackPointer.getcurrentscope()+1);
            StackPointer.setscopeIncr(0);
            args->CompileRec(destReg);
            statements->CompileRec(destReg);
            Symbol.endScope();
            StackPointer.setcurrentscope(StackPointer.getcurrentscope()-1);
        }else{
            statements->CompileRec(destReg);
        }

        StackPointer.setIncr(StackPointer.getIncr()-44);
        std::cout << "lw $s0, 4($sp)" << std::endl;
        std::cout << "lw $s1, 8($sp)" << std::endl;
        std::cout << "lw $s2, 12($sp)" << std::endl;
        std::cout << "lw $s3, 16($sp)" << std::endl;
        std::cout << "lw $s4, 20($sp)" << std::endl;
        std::cout << "lw $s5, 24($sp)" << std::endl;
        std::cout << "lw $s6, 28($sp)" << std::endl;
        std::cout << "lw $s7, 32($sp)" << std::endl;
        std::cout << "lw $fp, 36($sp)" << std::endl;
        std::cout << "lw $ra, 40($sp)" << std::endl;
        std::cout << "move $sp, $fp" << std::endl;
        std::cout << "jr $ra" << std::endl;
        if(Symbol.getScope()==0){
            std::cout << ".global " << getFunction() << std::endl;
        }
    }

    void print(std::ostream &dst) const
    {
        name->print(dst);
        dst<<"( ";
        dst<<" )";
        dst<<'\n';
        statements->print(dst);
    }

};
#endif