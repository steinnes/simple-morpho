%{
#include <string.h>
#include <string>
#include <iostream>
#include "tokens.h"

#define YYSTYPE string
int line = 1;
%}

%option c++
%option noyywrap
%option yylineno

INT 		[0-9]+
OP 		[+\-]
NEWLINE 	\r|\n|\r\n
WHITESPACE      [\n\r\ \t\b\012]

%%
{INT} { return(INT); }
{OP} { return(OP); }
{NEWLINE} { line++; }
{WHITESPACE} { }

%%


