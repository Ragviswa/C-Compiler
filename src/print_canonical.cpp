#include "ast.hpp"

SymbolTable Symbol;
StackPtr StackPointer;

int main()
{
    const Function *ast=parseAST();

    ast->print(std::cout);
    std::cout<<std::endl;

    return 0;
}
