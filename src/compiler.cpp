#include <iostream>
#include "ast.hpp"

SymbolTable Symbol;
StackPtr StackPointer;

int main(int argc, char *argv[]) {
    FILE *fileInput = fopen(argv[1],"r");
    std::cout << ".data" << std::endl;
    const Body *ast = parseAST(fileInput);
    ast->CompileRec("$v0");
    /*
    if(ast->getType() == "INT") {
        ast->CompileRec("$v0");
    }
    else if(ast->getType() == "FLOAT") {
        ast->CompileRec("$f0");
    }
    */
    return 0;
}