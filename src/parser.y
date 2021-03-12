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
  const Function *function;
  const Statement *stat;
  const StatementList *statlist;
  const Expression *expr;
  const DeclarationList *decllist;
  const Variable *variable;
  double number;
  std::string *string;
  TypeDef T_type;
}

%token T_TIMES T_DIVIDE T_PLUS T_MINUS T_MODULO
%token T_GREATERTHAN T_LESSTHAN T_GREATERTHANEQUAL T_LESSTHANEQUAL T_EQUALTO T_NOTEQUALTO
%token T_NOT T_LOGICAND T_LOGICOR
%token T_QUESTION T_COLON
%token T_ASSIGN T_SEMICOLON T_COMMA
%token T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET
%token T_INT T_RETURN T_WHILE T_IF T_ELSE
%token T_NUMBER T_VARIABLE

%type <stat> EXPR_STAT SEL_STAT LOOP_STAT JUMP_STAT STAT COMPOUND_STAT
%type <expr> EXPR CONDITIONAL LOGIC_OR LOGIC_AND EQUALITY RELAT ARITH TERM UNARY FACTOR
%type <number> T_NUMBER
%type <string> T_INT T_VARIABLE
%type <T_type> TYPE_DEF
%type <variable> DECL
%type <statlist> STAT_LIST
%type <decllist> DECL_LIST
%type <function> FUNCTION

%start PROGRAM

%%

PROGRAM             : FUNCTION                                              { g_root = $1; }

FUNCTION            : TYPE_DEF T_VARIABLE T_LBRACKET T_RBRACKET COMPOUND_STAT { $$ = new Function((new Variable($1, $2)), $5); }

COMPOUND_STAT       : T_LBRACE T_RBRACE                                     { $$ = new CompoundStatement(); }
                    | T_LBRACE STAT_LIST T_RBRACE                           { $$ = new CompoundStatement($2); }
                    | T_LBRACE DECL_LIST T_RBRACE                           { $$ = new CompoundStatement($2); }
                    | T_LBRACE DECL_LIST STAT_LIST T_RBRACE                 { $$ = new CompoundStatement($2, $3); }

STAT_LIST           : STAT                                                  { $$ = $1; }
                    | STAT_LIST STAT                                        { $$ = new StatementList($2, $1); }

DECL_LIST           : DECL                                                  { $$ = $1; }
                    | DECL_LIST DECL                                        { $$ = new DeclarationList($2, $1); }

STAT                : COMPOUND_STAT                                         { $$ = $1; }
                    | LOOP_STAT                                             { $$ = $1; }
                    | SEL_STAT                                              { $$ = $1; }
                    | EXPR_STAT                                             { $$ = $1; }
                    | JUMP_STAT                                             { $$ = $1; }

JUMP_STAT           : T_RETURN T_SEMICOLON                                    { $$ = new JumpStatement(); }
                    | T_RETURN EXPR T_SEMICOLON                               { $$ = new JumpStatement($2); }

LOOP_STAT           : T_WHILE T_LBRACKET EXPR T_RBRACKET STAT                 { $$ = new WhileLoop($3, $5); }

SEL_STAT            : T_IF T_LBRACKET EXPR T_RBRACKET STAT                  { $$ = new IfStatement($3, $5); }
                    | T_IF T_LBRACKET EXPR T_RBRACKET STAT T_ELSE STAT      { $$ = new IfStatement($3, $5, $7); }

EXPR_STAT           : T_SEMICOLON                                           { $$ = new ExpressionStatement(); }
                    | EXPR T_SEMICOLON                                      { $$ = new ExpressionStatement($1); }

DECL                : TYPE_DEF T_VARIABLE T_SEMICOLON                       { $$ = new Variable($1, $2); }
                    | TYPE_DEF T_VARIABLE T_ASSIGN EXPR T_SEMICOLON         { $$ = new Variable($1, $2, $4); }

EXPR                : CONDITIONAL                                           { $$ = $1; }
                    | T_VARIABLE T_ASSIGN EXPR                              {}

CONDITIONAL         : LOGIC_OR                                              { $$ = $1; }
                    | LOGIC_OR T_QUESTION EXPR T_COLON CONDITIONAL          { $$ = new TernaryOperator($1, $3, $5); }

LOGIC_OR            : LOGIC_AND                                             { $$ = $1; }
                    | LOGIC_OR T_LOGICOR LOGIC_AND                          { $$ = new OrLogic($1, $3); }

LOGIC_AND           : EQUALITY                                              { $$ = $1; }
                    | LOGIC_AND T_LOGICAND EQUALITY                         { $$ = new AndLogic($1, $3); }

EQUALITY            : RELAT                                                 { $$ = $1; }
                    | EQUALITY T_EQUALTO RELAT                              { $$ = new EqualOperator($1, $3); }
                    | EQUALITY T_NOTEQUALTO RELAT                           { $$ = new NotEqualOperator($1, $3); }

RELAT               : ARITH                                                 { $$ = $1; }
                    | RELAT T_GREATERTHAN ARITH                             { $$ = new GreaterThanOperator($1, $3); }
                    | RELAT T_LESSTHAN ARITH                                { $$ = new LessThanOperator($1, $3); }
                    | RELAT T_GREATERTHANEQUAL ARITH                        { $$ = new GreaterThanEqualOperator($1, $3); }
                    | RELAT T_LESSTHANEQUAL ARITH                           { $$ = new LessThanEqualOperator($1, $3); }

ARITH               : TERM                                                  { $$ = $1; }
                    | ARITH T_PLUS TERM                                     { $$ = new AddOperator($1, $3); }
                    | ARITH T_MINUS TERM                                    { $$ = new SubOperator($1, $3); }

TERM                : UNARY                                                 { $$ = $1; }
                    | TERM T_TIMES UNARY                                    { $$ = new MulOperator($1, $3); }
                    | TERM T_DIVIDE UNARY                                   { $$ = new DivOperator($1, $3); }
                    | TERM T_MODULO UNARY                                   { $$ = new ModOperator($1, $3); }

UNARY               : FACTOR                                                { $$ = $1; }
                    | T_MINUS FACTOR                                        { $$ = new NegOperator($2); }
                    | T_NOT FACTOR                                          { $$ = new NotLogic($2); }

TYPE_DEF            : T_INT                                                 { $$ = TypeDef::INT; }

FACTOR              : T_NUMBER                                              { $$ = new Number($1); }
                    | T_VARIABLE                                            { $$ = new std::string(*$1); }
                    | T_LBRACKET EXPR T_RBRACKET                            { $$ = $2; }

%%

const Function *g_root; // Definition of variable (to match declaration earlier)

const Function *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
