#include <iostream>
#include "ast.hpp"

SymbolTable Symbol;
StackPtr StackPointer;

int main(int argc, char *argv[]) {
    FILE *fileInput = fopen(argv[1],"r");
    std::cout << ".text" << std::endl;
    const Function *ast = parseAST(fileInput);
    ast->CompileRec("$v0");
    return 0;
}