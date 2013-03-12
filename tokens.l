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

VALUE 		[0-9\.]+
OP 		[+\-]
NEWLINE 	\r|\n|\r\n
WHITESPACE      [\n\r\ \t\b\012]

%%
{VALUE} 			{ return(VALUE); }
{OP} 				{ return(OP); }
{NEWLINE} 			{ line++; }
{WHITESPACE} 			{ }  // skip whitespaces
"("|")"|"{"|"}"|","|";"         return (int)yytext[0]; // delimiters

%%


