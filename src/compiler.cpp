#include <iostream>
#include "ast.hpp"

SymbolTable Symbol;
StackPtr StackPointer;

int main() {
    std::cout << ".text" << std::endl;
    const Function *ast = parseAST();
    ast->CompileRec("$v0");
    return 0;
}