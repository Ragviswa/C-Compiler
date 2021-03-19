#include "ast.hpp"

SymbolTable Symbol;
StackPtr StackPointer;

int main()
{
    const Function *ast=parseAST();
    std::cout << ".text" << std::endl;
    ast->CompileRec("$v0");
    std::cout<<std::endl;

    return 0;
}
