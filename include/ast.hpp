#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_expression.hpp"
#include "ast/ast_function.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_operators.hpp"
#include "ast/ast_statement.hpp"
#include "ast/ast_unary.hpp"

extern const Function *parseAST();

#endif
