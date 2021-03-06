#ifndef ast_function_hpp
#define ast_function_hpp

#include "ast_primitives.hpp"
#include "ast_statement.hpp"
#include "ast_expression.hpp"

class Function
{
private:
    
    FunctionStorage *name; // function name
    DeclarationListPtr args;
    StatementPtr statements;
public:
    Function(FunctionStorage *_name, DeclarationListPtr _args = nullptr, StatementPtr _statements = nullptr)
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

    const std::string getType() const
    {
        return name->getType();
    }

    DeclarationListPtr getArgs() const
    { return args; }

    void CompileRec(std::string destReg) const {
        if(!StackPointer.gettext()){
            std::cout << ".text" << std::endl;
            StackPointer.settext(1);
        }
        if(statements!=nullptr){
            name->CompileRec(destReg);
        }else{
            name->CompileRec("$0");
        }
        std::string functionType = getType();
        if(statements!=nullptr){
            Symbol.setfuncscope(Symbol.getfuncscope()+1);
            Symbol.setfuncstart(name->getId());
            Symbol.setfuncend(name->getId()+"end");
            std::cout << name->getId() << ":" << std::endl;
            std::cout << "move $t7, $fp" << std::endl;
            std::cout << "move $fp, $sp" << std::endl;
            std::cout << "addiu $sp, $sp, -44" << std::endl;
            std::cout << "sw $s0, 0($sp)" << std::endl;
            std::cout << "sw $s1, 4($sp)" << std::endl;
            std::cout << "sw $s2, 8($sp)" << std::endl;
            std::cout << "sw $s3, 12($sp)" << std::endl;
            std::cout << "sw $s4, 16($sp)" << std::endl;
            std::cout << "sw $s5, 20($sp)" << std::endl;
            std::cout << "sw $s6, 24($sp)" << std::endl;
            std::cout << "sw $s7, 28($sp)" << std::endl;
            std::cout << "sw $fp, 32($sp)" << std::endl;
            std::cout << "sw $ra, 36($sp)" << std::endl;
            std::cout << "sw $t7, 40($sp)" << std::endl;
            StackPointer.setIncr(StackPointer.getIncr()+44);

            if(args!=nullptr){
                Symbol.newScope();
                StackPointer.setcurrentscope(StackPointer.getcurrentscope()+1);
                StackPointer.setscopeIncr(0);
                args->CompileRec(destReg);
                if(functionType == "FLOAT" || functionType == "DOUBLE") {
                    statements->CompileRec("$f0");
                }
                else {
                    statements->CompileRec(destReg);
                }
                std::cout << name->getId() << "end:" << std::endl;
                for(int i = StackPointer.getcurrentscope(); i<50; i++){
                    if(StackPointer.getscopeIncr(i)!=0){
                        std::cout << "addiu $sp, $sp, " << StackPointer.getscopeIncr(i) << std::endl;
                        StackPointer.setIncr(StackPointer.getIncr()-StackPointer.getscopeIncr(i));
                    }
                }
                StackPointer.setscopeIncr(0);
                StackPointer.setcurrentscope(StackPointer.getcurrentscope()-1);
                Symbol.endScope();
            }else{
                if(functionType == "FLOAT" || functionType == "DOUBLE") {
                    statements->CompileRec("$f0");
                }
                else {
                    statements->CompileRec(destReg);
                }
                std::cout << name->getId() << "end:" << std::endl;
                for(int i = StackPointer.getcurrentscope() + 1; i<50; i++){
                    if(StackPointer.getscopeIncr(i)!=0){
                        std::cout << "addiu $sp, $sp, " << StackPointer.getscopeIncr(i) << std::endl;
                        StackPointer.setIncr(StackPointer.getIncr()-StackPointer.getscopeIncr(i));
                    }
                }
            }
            StackPointer.setIncr(StackPointer.getIncr()-44);
            std::cout << "lw $s0, 0($sp)" << std::endl;
            std::cout << "lw $s1, 4($sp)" << std::endl;
            std::cout << "lw $s2, 8($sp)" << std::endl;
            std::cout << "lw $s3, 12($sp)" << std::endl;
            std::cout << "lw $s4, 16($sp)" << std::endl;
            std::cout << "lw $s5, 20($sp)" << std::endl;
            std::cout << "lw $s6, 24($sp)" << std::endl;
            std::cout << "lw $s7, 28($sp)" << std::endl;
            std::cout << "lw $fp, 32($sp)" << std::endl;
            std::cout << "lw $ra, 36($sp)" << std::endl;
            std::cout << "lw $t7, 40($sp)" << std::endl;
            std::cout << "move $sp, $fp" << std::endl;
            std::cout << "move $fp, $t7" << std::endl;
            std::cout << "jr $ra" << std::endl;
            Symbol.setfuncscope(Symbol.getfuncscope()-1);
            StackPointer.setfreturn(0);
            if(Symbol.getScope()==0){
            std::cout << ".global " << name->getId() << std::endl;
            }
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

class Body
{
private:

    Function *func = nullptr;
    Variable  *decl = nullptr;
    Body *body = nullptr;

public:
    Body(Function *_func, Body *_body = nullptr)
        : func(_func)
        , body(_body)
    {}

    Body(Variable *_decl, Body *_body = nullptr) 
        : decl(_decl)
        , body(_body)
    {}
    virtual ~Body()
    {
        delete func,
        delete decl,
        delete body;
    }

    Function *getFunc() const
    { return func; }
    Variable *getDecl() const
    { return decl;}
    Body *getBody() const
    { return body; }

    void CompileRec(std::string destReg) const{
        if(getFunc() != nullptr){
            getFunc()->CompileRec(destReg);
        }else if(getDecl() != nullptr){
            getDecl()->CompileRec(destReg);
        }
        if(getBody()!=nullptr){
            getBody()->CompileRec(destReg);
        }
    }

};
#endif