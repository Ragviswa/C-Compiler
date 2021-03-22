#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_expression.hpp"
#include "ast/ast_function.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_operators.hpp"
#include "ast/ast_statement.hpp"
#include "ast/ast_unary.hpp"
#include "ast/symbol_table.hpp"

extern const Body *parseAST(FILE *fileInput);
//extern const Body *parseAST();
extern SymbolTable Symbol;
extern StackPtr StackPointer;

#endif
