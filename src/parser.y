%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Function *g_root; // A way of getting the AST out

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
  const Statement *stat;
  double number;
  std::string *string;
}

%token T_TIMES T_DIVIDE T_PLUS T_MINUS T_MODULO T_QUESTION T_COLON
%token T_GREATERTHAN T_LESSTHAN T_GREATERTHANEQUAL T_LESSTHANEQUAL T_EQUALTO T_NOTEQUALTO T_NOT T_LOGICAND T_LOGICOR
%token T_LBRACKET T_RBRACKET T_LBRACE T_RBRACE T_SEMICOLON T_ASSIGN T_COMMA
%token T_INT T_WHILE T_IF T_ELSE T_RETURN
%token T_NUMBER T_VARIABLE

%type <expr> EXPR CONDITIONAL LOGIC_OR LOGIC_AND EQUALITY RELAT ARITH TERM UNARY FACTOR
%type <number> T_NUMBER
%type <string> T_VARIABLE T_WHILE T_IF T_ELSE T_RETURN T_INT

%start FUNCTION

%%

FUNCTION            : TYPE_DEF T_VARIABLE T_LBRACKET T_RBRACKET COMPOUND_STAT

COMPOUND_STAT       : T_LBRACE T_RBRACE
                    | T_LBRACE STAT_LIST T_RBRACE
                    | T_LBRACE DECL_LIST T_RBRACE
                    | T_LBRACE DECL_LIST STAT_LIST T_RBRACE

STAT_LIST           : STAT
                    | STAT_LIST STAT

DECL_LIST           : DECL
                    | DECL_LIST DECL

STAT                : COMPOUND_STAT
                    | LOOP_STAT
                    | SEL_STAT
                    | EXPR_STAT
                    | JUMP_STAT

JUMP_STAT           : RETURN T_SEMICOLON
                    | RETURN EXPR T_SEMICOLON

LOOP_STAT           : WHILE T_LBRACKET EXPR T_RBRACKET STAT

SEL_STAT            : T_IF T_LBRACKET EXPR T_RBRACKET STAT
                    | T_IF T_LBRACKET EXPR T_RBRACKET STAT T_ELSE STAT

EXPR_STAT           : T_SEMICOLON
                    | EXPR T_SEMICOLON

DECL                : TYPE_DEF T_VARIABLE T_SEMICOLON
                    | TYPE_DEF T_VARIABLE T_ASSIGN EXPR T_SEMICOLON

EXPR                : CONDITIONAL
                    | T_VARIABLE T_ASSIGN EXPR

CONDITIONAL         : LOGIC_OR                                              { $$ = $1; }
                    | LOGIC_OR T_QUESTION EXPR T_COLON CONDITIONAL          { $$ = new TernaryOperator($1, $3, $5); }

LOGIC_OR            : LOGIC_AND                     { $$ = $1; }
                    | LOGIC_OR T_LOGICOR LOGIC_AND  { $$ = new OrLogic($1, $3); }

LOGIC_AND           : EQUALITY                      { $$ = $1; }
                    | LOGIC_AND T_LOGICAND EQUALITY { $$ = new AndLogic($1, $3); }

EQUALITY            : RELAT                         { $$ = $1; }
                    | EQUALITY T_EQUALTO RELAT      { $$ = new EqualOperator($1, $3); }
                    | EQUALITY T_NOTEQUALTO RELAT   { $$ = new NotEqualOperator($1, $3); }

RELAT               : ARITH                         { $$ = $1; }
                    | RELAT T_GREATERTHAN ARITH     { $$ = new GreaterThanOperator($1, $3); }
                    | RELAT T_LESSTHAN ARITH        { $$ = new LessThanOperator($1, $3); }
                    | RELAT T_GREATERTHANEQUAL ARITH{ $$ = new GreaterThanEqualOperator($1, $3); }
                    | RELAT T_LESSTHANEQUAL ARITH   { $$ = new LessThanEqualOperator($1, $3); }

ARITH               : TERM                          { $$ = $1; }
                    | ARITH T_PLUS TERM             { $$ = new AddOperator($1, $3); }
                    | ARITH T_MINUS TERM            { $$ = new SubOperator($1, $3); }

TERM                : UNARY                         { $$ = $1; }
                    | TERM T_TIMES UNARY            { $$ = new MulOperator($1, $3); }
                    | TERM T_DIVIDE UNARY           { $$ = new DivOperator($1, $3); }
                    | TERM T_MODULO UNARY           { $$ = new ModOperator($1, $3); }

UNARY               : FACTOR                        { $$ = $1; }
                    | T_MINUS FACTOR                { $$ = new NegOperator($2); }
                    | T_NOT FACTOR                  { $$ = new NotLogic($2); }

TYPE_DEF            : T_INT                         { $$ = new Integer()}

FACTOR              : T_NUMBER                      { $$ = new Number( $1 ); }
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
