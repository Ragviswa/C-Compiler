%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
%}

%%
[*]             { return T_TIMES; }
[/]             { return T_DIVIDE; }
[+]             { return T_PLUS; }
[-]             { return T_MINUS; }
[%]             { return T_MODULO; }

[>]             { return T_GREATERTHAN; }
[<]             { return T_LESSTHAN; }
[>][=]          { return T_GREATERTHANEQUAL; }
[<][=]          { return T_LESSTHANEQUAL; }
[=][=]          { return T_EQUALTO; }
[!][=]          { return T_NOTEQUALTO; }

[<][<]          { return T_LSHIFT; }
[>][>]          { return T_RSHIFT; }

[&]             { return T_AND; }
[|]             { return T_IOR; }
[\^]            { return T_XOR; }

[&][&]          { return T_LOGICAND; }
[|][|]          { return T_LOGICOR; }
[!]             { return T_NOT; }

[?]             { return T_QUESTION; }
[:]             { return T_COLON; }
[+][+]          { return T_INCR; }
[-][-]          { return T_DECR; }

[=]             { return T_ASSIGN; }
[+][=]          { return T_ADDASSIGN; }
[-][=]          { return T_SUBASSIGN; }
[/][=]          { return T_DIVASSIGN; }
[*][=]          { return T_MULASSIGN; }
[%][=]          { return T_MODASSIGN; }
[<][<][=]       { return T_LEFASSIGN; }
[>][>][=]       { return T_RIGASSIGN; }
[&][=]          { return T_ANDASSIGN; }
[|][=]          { return T_XORASSIGN; }
[\^][=]         { return T_ORASSIGN; }

[;]             { return T_SEMICOLON; }
[,]             { return T_COMMA; }

[{]             { return T_LBRACE; }
[}]             { return T_RBRACE; }
[(]             { return T_LBRACKET; }
[)]             { return T_RBRACKET; }
[\[]             { return T_LSBRACKET; }
[\]]             { return T_RSBRACKET; }

int             { return T_INT; }
float           { return T_FLOAT; }
double          { return T_DOUBLE; }
return          { return T_RETURN; }
while           { return T_WHILE; }
if              { return T_IF; }
else            { return T_ELSE; }
for             { return T_FOR; }
switch          { return T_SWITCH; }
continue        { return T_CONTINUE; }
break           { return T_BREAK; }
case            { return T_CASE; }
enum            { return T_ENUM; }

[0-9]+                         { yylval.number=strtod(yytext, 0); return T_NUMBER_INT; }
[0-9]+[.]([0-9]+)*             { yylval.number=strtod(yytext, 0); return T_NUMBER_DOUBLE; }
[a-zA-Z_][a-zA-Z_\d]*          { yylval.string=new std::string(yytext); return T_VARIABLE; }

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token\n"); exit(1); }
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  fprintf(stderr, "line %d: %s\n", yylineno, s);
  exit(1);
}