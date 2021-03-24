%code requires{
  #include "ast.hpp"

  #include <cassert>

  extern const Body *g_root; // A way of getting the AST out
  extern FILE* yyin;
  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  Body *body;
  Function *function;
  const Statement *stat;
  const Expression *expr;
  const BlockList *blocklist;
  const DeclarationList *decllist;
  const ExpressionList *arglist;
  EnumList *enumlist;
  StructMemberList *struclist;
  Variable *variable;
  Array *array;
  double number;
  std::string *string;
  TypeDef T_type;
}

%token T_TIMES T_DIVIDE T_PLUS T_MINUS T_MODULO
%token T_GREATERTHAN T_LESSTHAN T_GREATERTHANEQUAL T_LESSTHANEQUAL T_EQUALTO T_NOTEQUALTO
%token T_LSHIFT T_RSHIFT
%token T_AND T_IOR T_XOR
%token T_NOT T_LOGICAND T_LOGICOR
%token T_QUESTION T_COLON T_INCR T_DECR
%token T_ASSIGN T_ADDASSIGN T_SUBASSIGN T_DIVASSIGN T_MULASSIGN T_MODASSIGN T_LEFASSIGN T_RIGASSIGN T_ANDASSIGN T_XORASSIGN T_ORASSIGN
%token T_SEMICOLON T_COMMA
%token T_LBRACE T_RBRACE T_LBRACKET T_RBRACKET T_LSBRACKET T_RSBRACKET
%token T_INT T_FLOAT T_DOUBLE T_CHAR T_RETURN T_WHILE T_IF T_ELSE T_FOR T_SWITCH T_CONTINUE T_BREAK T_CASE T_DEFAULT T_ENUM T_STRUCT
%token T_NUMBER_INT T_NUMBER_DOUBLE T_VARIABLE T_CHAR_DATA T_STRING_DATA T_STRUCTVAR
%token T_SIZEOF

%type <stat> EXPR_STAT SEL_STAT LOOP_STAT JUMP_STAT LABL_STAT STAT COMPOUND_STAT
%type <expr> EXPR CONDITIONAL LOGIC_OR LOGIC_AND IOR_EXPR XOR_EXPR AND_EXPR EQUALITY RELAT SHIFT ARITH TERM UNARY POST FACTOR
%type <number> T_NUMBER_INT T_NUMBER_DOUBLE
%type <string> T_INT T_FLOAT T_DOUBLE T_CHAR T_VARIABLE ASSIGNOP T_CHAR_DATA T_STRING_DATA T_STRUCTVAR
%type <T_type> TYPE_DEF
%type <variable> DECL ARG ENUM STRUCT_MEMBER
%type <blocklist> BLOCK_ITEM_LIST
%type <decllist> ARG_LIST
%type <enumlist> ENUM_LIST
%type <struclist> STRUCT_MEMBER_LIST
%type <arglist> EXPR_LIST
%type <function> FUNCTION
%type <body> BODY

%start PROGRAM

%%

PROGRAM             : BODY                                              { g_root = $1; }

BODY                : FUNCTION                                          { $$ = new Body($1); }
                    | DECL                                              { $$ = new Body($1); }
                    | FUNCTION BODY                                     { $$ = new Body($1, $2); }
                    | DECL BODY                                         { $$ = new Body($1, $2); }

FUNCTION            : TYPE_DEF T_VARIABLE T_LBRACKET T_RBRACKET COMPOUND_STAT           { $$ = new Function((new FunctionStorage($1, $2)), nullptr, $5); }
                    | TYPE_DEF T_VARIABLE T_LBRACKET ARG_LIST T_RBRACKET COMPOUND_STAT  { $$ = new Function((new FunctionStorage($1, $2)), $4, $6); }
                    | TYPE_DEF T_VARIABLE T_LBRACKET T_RBRACKET T_SEMICOLON             { $$ = new Function((new FunctionStorage($1, $2)), nullptr, nullptr); }
                    | TYPE_DEF T_VARIABLE T_LBRACKET ARG_LIST T_RBRACKET T_SEMICOLON    { $$ = new Function((new FunctionStorage($1, $2)), $4, nullptr); }

ARG_LIST            : ARG                                                   { $$ = new DeclarationList($1, nullptr); }
                    | ARG T_COMMA ARG_LIST                                  { $$ = new DeclarationList($1, $3); }

ARG                 : TYPE_DEF T_VARIABLE                                   { $$ = new Variable($1, $2, DeclType::ARG); }
                    | TYPE_DEF T_LSBRACKET T_RSBRACKET T_VARIABLE           { $$ = new Array($1, $4); }
                    | TYPE_DEF T_TIMES T_VARIABLE                           { $$ = new Pointer($1, $3, DeclType::ARG); }

COMPOUND_STAT       : T_LBRACE T_RBRACE                                     { $$ = new CompoundStatement(); }
                    | T_LBRACE BLOCK_ITEM_LIST T_RBRACE                     { $$ = new CompoundStatement($2); }

BLOCK_ITEM_LIST     : STAT                                                  { $$ = new BlockList($1, nullptr); }
                    | DECL                                                  { $$ = new BlockList($1, nullptr); }
                    | STAT BLOCK_ITEM_LIST                                  { $$ = new BlockList($1, $2); }
                    | DECL BLOCK_ITEM_LIST                                  { $$ = new BlockList($1, $2); }

STAT                : COMPOUND_STAT                                         { $$ = $1; }
                    | LOOP_STAT                                             { $$ = $1; }
                    | SEL_STAT                                              { $$ = $1; }
                    | EXPR_STAT                                             { $$ = $1; }
                    | JUMP_STAT                                             { $$ = $1; }
                    | LABL_STAT                                             { $$ = $1; }

LABL_STAT           : T_CASE CONDITIONAL T_COLON STAT                       { $$ = new LabelStatement($4, $2); }
                    | T_DEFAULT T_COLON STAT                                { $$ = new LabelStatement($3); }

JUMP_STAT           : T_CONTINUE T_SEMICOLON                                { $$ = new ContinueStatement(); }
                    | T_BREAK T_SEMICOLON                                   { $$ = new BreakStatement(); }
                    | T_RETURN T_SEMICOLON                                  { $$ = new ReturnStatement(); }
                    | T_RETURN EXPR T_SEMICOLON                             { $$ = new ReturnStatement($2); }

LOOP_STAT           : T_WHILE T_LBRACKET EXPR T_RBRACKET STAT                                 { $$ = new WhileLoop($3, $5); }
                    | T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON EXPR T_RBRACKET STAT { $$ = new ForLoop($3, $5, $7, $9); }
                    | T_FOR T_LBRACKET DECL EXPR T_SEMICOLON EXPR T_RBRACKET STAT             { $$ = new ForLoop($3, $4, $6, $8); }

SEL_STAT            : T_IF T_LBRACKET EXPR T_RBRACKET STAT                  { $$ = new IfStatement($3, $5); }
                    | T_IF T_LBRACKET EXPR T_RBRACKET STAT T_ELSE STAT      { $$ = new IfStatement($3, $5, $7); }
                    | T_SWITCH T_LBRACKET EXPR T_RBRACKET STAT              { $$ = new SwitchStatement($3, $5);}

EXPR_STAT           : T_SEMICOLON                                           { $$ = new ExpressionStatement(); }
                    | EXPR T_SEMICOLON                                      { $$ = new ExpressionStatement($1); }

ENUM_LIST           : ENUM                                                  { $$ = new EnumList($1); }
                    | ENUM T_COMMA ENUM_LIST                                { $$ = new EnumList($1, $3); }

ENUM                : T_VARIABLE                                            { $$ = new Variable(TypeDef::ENUM, $1, DeclType::DECL); }
                    | T_VARIABLE T_ASSIGN CONDITIONAL                       { $$ = new Variable(TypeDef::ENUM, $1, DeclType::DECL, $3); }

STRUCT_MEMBER_LIST  : STRUCT_MEMBER                                                                     { $$ = new StructMemberList($1); }
                    | STRUCT_MEMBER STRUCT_MEMBER_LIST                                                  { $$ = new StructMemberList($1, $2); }

STRUCT_MEMBER       : TYPE_DEF T_VARIABLE T_SEMICOLON                                                   { $$ = new Variable($1, $2, DeclType::STRUCT); }
                    | TYPE_DEF T_VARIABLE T_LSBRACKET T_NUMBER_INT T_RSBRACKET T_SEMICOLON              { $$ = new Array($1, $2, $4); }

DECL                : TYPE_DEF T_VARIABLE T_SEMICOLON                                                   { $$ = new Variable($1, $2, DeclType::DECL); }
                    | TYPE_DEF T_VARIABLE T_ASSIGN EXPR T_SEMICOLON                                     { $$ = new Variable($1, $2, DeclType::DECL, $4); }
                    | TYPE_DEF T_VARIABLE T_LSBRACKET T_NUMBER_INT T_RSBRACKET T_SEMICOLON              { $$ = new Array($1, $2, $4); }
                    | TYPE_DEF T_VARIABLE T_LSBRACKET CONDITIONAL T_RSBRACKET T_ASSIGN EXPR T_SEMICOLON {}
                    | TYPE_DEF T_TIMES T_VARIABLE T_SEMICOLON                                           { $$ = new Pointer($1, $3, DeclType::DECL); }
                    | TYPE_DEF T_TIMES T_VARIABLE T_ASSIGN EXPR T_SEMICOLON                             { $$ = new Pointer($1, $3, DeclType::DECL, $5); }
                    | T_CHAR T_TIMES T_VARIABLE T_ASSIGN T_STRING_DATA T_SEMICOLON                      { $$ = new Pointer($3, $5); }
                    | T_ENUM T_LBRACE ENUM_LIST T_RBRACE T_SEMICOLON                                    { $$ = new EnumKeyword(nullptr, $3); }
                    | T_ENUM T_VARIABLE T_LBRACE ENUM_LIST T_RBRACE T_SEMICOLON                         { $$ = new EnumKeyword($2, $4); }
                    | T_ENUM T_VARIABLE                                                                 { $$ = new EnumKeyword($2, nullptr); }
                    | T_STRUCT T_VARIABLE T_LBRACE STRUCT_MEMBER_LIST T_RBRACE T_SEMICOLON              { $$ = new StructStorage($2, $4); }
                    | T_STRUCT T_VARIABLE T_VARIABLE T_SEMICOLON                                        { $$ = new Variable($2, $3, DeclType::DECL); }

EXPR                : CONDITIONAL                                           { $$ = $1; }
                    | T_VARIABLE ASSIGNOP EXPR                              { $$ = new Variable($1, $2, $3);}
                    | T_STRUCTVAR ASSIGNOP EXPR                             { $$ = new Variable($1, $2, $3);}
                    | T_VARIABLE T_LSBRACKET EXPR T_RSBRACKET ASSIGNOP EXPR { $$ = new Array($1, $3, $5, $6); }
                    | T_VARIABLE T_LSBRACKET EXPR T_RSBRACKET T_INCR        { $$ = new Array($1, $3, new std::string("++"), nullptr); }
                    | T_VARIABLE T_LSBRACKET EXPR T_RSBRACKET T_DECR        { $$ = new Array($1, $3, new std::string("--"), nullptr); }
                    | T_VARIABLE T_INCR                                     { $$ = new Variable($1, new std::string("++"), nullptr); }
                    | T_VARIABLE T_DECR                                     { $$ = new Variable($1, new std::string("--"), nullptr); }
                    | T_STRUCTVAR T_INCR                                    { $$ = new Variable($1, new std::string("++"), nullptr); }
                    | T_STRUCTVAR T_DECR                                    { $$ = new Variable($1, new std::string("--"), nullptr); }
                    | T_TIMES T_VARIABLE ASSIGNOP EXPR                      { $$ = new Pointer($2, $3, $4, true); }

ASSIGNOP            : T_ASSIGN                                              { $$ = new std::string("="); }
                    | T_ADDASSIGN                                           { $$ = new std::string("+="); }
                    | T_SUBASSIGN                                           { $$ = new std::string("-="); }
                    | T_DIVASSIGN                                           { $$ = new std::string("/="); }
                    | T_MULASSIGN                                           { $$ = new std::string("*="); }
                    | T_MODASSIGN                                           { $$ = new std::string("%="); }
                    | T_LEFASSIGN                                           { $$ = new std::string("<<="); }
                    | T_RIGASSIGN                                           { $$ = new std::string(">>="); }
                    | T_ANDASSIGN                                           { $$ = new std::string("&="); }
                    | T_XORASSIGN                                           { $$ = new std::string("^="); }
                    | T_ORASSIGN                                            { $$ = new std::string("|="); }

CONDITIONAL         : LOGIC_OR                                              { $$ = $1; }
                    | LOGIC_OR T_QUESTION EXPR T_COLON CONDITIONAL          { $$ = new TernaryOperator($1, $3, $5); }

LOGIC_OR            : LOGIC_AND                                             { $$ = $1; }
                    | LOGIC_OR T_LOGICOR LOGIC_AND                          { $$ = new OrLogic($1, $3); }

LOGIC_AND           : IOR_EXPR                                              { $$ = $1; }
                    | LOGIC_AND T_LOGICAND IOR_EXPR                         { $$ = new AndLogic($1, $3); }

IOR_EXPR            : XOR_EXPR                                              { $$ = $1; }
                    | IOR_EXPR T_IOR XOR_EXPR                               { $$ = new OrOperator($1, $3); }

XOR_EXPR            : AND_EXPR                                              { $$ = $1; }
                    | XOR_EXPR T_XOR AND_EXPR                               { $$ = new XorOperator($1, $3); }

AND_EXPR            : EQUALITY                                              { $$ = $1; }
                    | AND_EXPR T_AND EQUALITY                               { $$ = new AndOperator($1, $3); }

EQUALITY            : RELAT                                                 { $$ = $1; }
                    | EQUALITY T_EQUALTO RELAT                              { $$ = new EqualOperator($1, $3); }
                    | EQUALITY T_NOTEQUALTO RELAT                           { $$ = new NotEqualOperator($1, $3); }

RELAT               : SHIFT                                                 { $$ = $1; }
                    | RELAT T_GREATERTHAN SHIFT                             { $$ = new GreaterThanOperator($1, $3); }
                    | RELAT T_LESSTHAN SHIFT                                { $$ = new LessThanOperator($1, $3); }
                    | RELAT T_GREATERTHANEQUAL SHIFT                        { $$ = new GreaterThanEqualOperator($1, $3); }
                    | RELAT T_LESSTHANEQUAL SHIFT                           { $$ = new LessThanEqualOperator($1, $3); }

SHIFT               : ARITH                                                 { $$ = $1; }  
                    | SHIFT T_RSHIFT ARITH                                  { $$ = new RightShift($1, $3); }
                    | SHIFT T_LSHIFT ARITH                                  { $$ = new LeftShift($1, $3); }

ARITH               : TERM                                                  { $$ = $1; }
                    | ARITH T_PLUS TERM                                     { $$ = new AddOperator($1, $3); }
                    | ARITH T_MINUS TERM                                    { $$ = new SubOperator($1, $3); }

TERM                : UNARY                                                 { $$ = $1; }
                    | TERM T_TIMES UNARY                                    { $$ = new MulOperator($1, $3); }
                    | TERM T_DIVIDE UNARY                                   { $$ = new DivOperator($1, $3); }
                    | TERM T_MODULO UNARY                                   { $$ = new ModOperator($1, $3); }

UNARY               : FACTOR                                                { $$ = $1; }
                    | T_MINUS POST                                          { $$ = new NegOperator($2); }
                    | T_PLUS POST                                           { $$ = new PosOperator($2); }
                    | T_NOT POST                                            { $$ = new NotLogic($2); }

POST                : FACTOR                                                { $$ = $1; }

EXPR_LIST           : EXPR                                                  { $$ = new ExpressionList($1); }
                    | EXPR T_COMMA EXPR_LIST                                { $$ = new ExpressionList($1, $3); }

TYPE_DEF            : T_INT                                                 { $$ = TypeDef::INT; }
                    | T_FLOAT                                               { $$ = TypeDef::FLOAT; }
                    | T_DOUBLE                                              { $$ = TypeDef::DOUBLE; }
                    | T_CHAR                                                { $$ = TypeDef::CHAR; }

FACTOR              : T_NUMBER_INT                                          { $$ = new Number_INT($1); }
                    | T_NUMBER_DOUBLE                                       { $$ = new Number_DOUBLE($1); }
                    | T_VARIABLE                                            { $$ = new Variable($1); }
                    | T_CHAR_DATA                                           { $$ = new SingleChar($1); }
                    | T_VARIABLE T_LBRACKET T_RBRACKET                      { $$ = new FunctionStorage($1); }
                    | T_VARIABLE T_LBRACKET EXPR_LIST T_RBRACKET            { $$ = new FunctionStorage($1, $3); }
                    | T_LBRACKET EXPR T_RBRACKET                            { $$ = $2; }
                    | T_VARIABLE T_LSBRACKET EXPR T_RSBRACKET               { $$ = new Array($1, $3); }
                    | T_SIZEOF T_LBRACKET EXPR T_RBRACKET                   { $$ = new SizeOf($3); }
                    | T_SIZEOF T_LBRACKET TYPE_DEF T_RBRACKET               { $$ = new SizeOf($3); }
                    | T_TIMES T_VARIABLE                                    { $$ = new Pointer($2, true, false); }
                    | T_AND T_VARIABLE                                      { $$ = new Variable($2, true); }
                    | T_STRUCTVAR                                           { $$ = new Variable($1); }

%%
// Keep in mind Variable is creating a new Variable instead of pointing to an old declaration
// Currently Compound Statement is incorrectly parsing as it requires the order to be a declaration list then a statement list,
// but based on our understanding, it should be able to do it in any order
const Body *g_root; // Definition of variable (to match declaration earlier)


const Body *parseAST(FILE *inputFile)
{
  g_root=0;
  yyin = inputFile;
  yyparse();
  return g_root;
}

/*
const Body *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
*/
