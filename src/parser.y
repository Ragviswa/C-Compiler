%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Expression *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const Expression *expr;
  double number;
  std::string *string;
}

%token T_TIMES T_DIVIDE T_PLUS T_MINUS T_MODULO
%token T_GREATERTHAN T_LESSTHAN T_GREATERTHANEQUAL T_LESSTHANEQUAL T_EQUALTO T_NOTEQUALTO T_NOT T_LOGICAND T_LOGICOR
%token T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_ASSIGN
%token T_INT T_WHILE T_IF T_ELSE T_RETURN
%token T_NUMBER T_VARIABLE

%type <expr> EXPR TERM UNARY FACTOR
%type <number> T_NUMBER
%type <int> T_INT
%type <string> T_VARIABLE T_INT T_WHILE T_IF T_ELSE T_RETURN

%start STATEMENT

%%

FUNCTION  : T_INT T_VARIABLE T_RBRACKET T_LBRACKET T_LBRACE STATEMENT T_RBRACE      { g_root = $1; }

STATEMENT : EXPR T_SEMICOLON                                                        { g_root = $1; }
        | T_RETURN EXPR T_SEMICOLON                                      { $$ = new Operator($1, $3); }       

        | T_IF T_LBRACKET EXPR T_RBRACKET                                { $$ = new Operator($1, $3); }
        | T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT                      { $$ = new Operator($1, $3); }
        | T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT T_ELSE STATEMENT     { $$ = new Operator($1, $3); }

        | WHILE T_LBRACKET EXPR T_RBRACKET                               { $$ = new Operator($1, $3); }
        | WHILE T_LBRACKET EXPR T_RBRACKET STATEMENT                     { $$ = new Operator($1, $3); }

        | T_INT T_VARIABLE T_SEMICOLON                                   { $$ = new Operator($1, $3); }
        | T_INT T_VARIABLE T_ASSIGN EXPR T_SEMICOLON                     { $$ = new Operator($1, $3); }

EXPR    : RELAT                         { $$ = $1; }
        | LOGICAL T_LOGICAND RELAT      { $$ = new Operator($1, $3); }
        | LOGICAL T_LOGICOR RELAT       { $$ = new Operator($1, $3); }

RELAT   : ARITH                         { $$ = $1; }
        | RELAT T_GREATERTHAN ARITH     { $$ = new Operator($1, $3); }
        | RELAT T_LESSTHAN ARITH        { $$ = new Operator($1, $3); }
        | RELAT T_GREATERTHANEQUAL ARITH{ $$ = new Operator($1, $3); }
        | RELAT T_LESSTHANEQUAL ARITH   { $$ = new Operator($1, $3); }
        | RELAT T_EQUALTO ARITH         { $$ = new Operator($1, $3); }
        | RELAT T_NOTEQUALTO ARITH      { $$ = new Operator($1, $3); }


ARITH   : TERM                          { $$ = $1; }
        | ARITH T_PLUS TERM             { $$ = new AddOperator($1, $3); }
        | ARITH T_MINUS TERM            { $$ = new SubOperator($1, $3); }


TERM    : NOT                           { $$ = $1; }
        | TERM T_TIMES UNARY            { $$ = new MulOperator($1, $3); }
        | TERM T_DIVIDE UNARY           { $$ = new DivOperator($1, $3); }

NOT     : UNARY                         { $$ = $1; }
        | T_NOT UNARY                   { $$ = new Operator($1, $3); }

UNARY   : FACTOR                        { $$ = $1; }
        | T_MINUS FACTOR                { $$ = new NegOperator($2); }

FACTOR  : T_NUMBER                      { $$ = new Number( $1 ); }
        | T_VARIABLE                    { $$ = new Variable( $1 ); }
        | T_LBRACKET EXPR T_RBRACKET    { $$ = $2; }


%%

const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
