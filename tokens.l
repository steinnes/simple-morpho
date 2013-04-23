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
FLOAT 		[0-9]*\.[0-9]+
STRING		\"[^"]*\"
TRUE		"true"|"TRUE"
FALSE		"false"|"FALSE"
OP 		[+\-\*<>]
NEWLINE 	\r|\n|\r\n
WHITESPACE      [\n\r\ \t\b\012]
ID		[_a-zA-Z]+[a-zA-Z0-9_]*
ASSIGN		"="

OR		"||"
AND		"&&"
NOT		"!"

%%
{INT} 		{ return(INT); }
{FLOAT} 	{ return(FLOAT); }
{STRING} 	{ return(STRING); }
{TRUE} 		{ return(TRUE); }
{FALSE} 	{ return(FALSE); }
{ASSIGN}	{ return(ASSIGN); }
{OP} 		{ return(OP); }
{OR}		{ return(OR); }
{AND}		{ return(AND); }
{NOT}		{ return(NOT); }
"fun"		{ return(FUN); }
"val"		{ return(VAL); }
"var"		{ return(VAR); }
"if"		{ return(IF); }
"while"		{ return(WHILE); }
"return"	{ return(RETURN); }
{NEWLINE} 	{ line++; }
{WHITESPACE} 	{ }  // skip whitespaces
"{"		{ return(LBRACE); }
"}"		{ return(RBRACE); }
","		{ return(COMMA); }
";"		{ return(SEMICOLON); }
"("		{ return(LPAREN); }
")"		{ return(RPAREN); }
{ID}		{ return(ID); }
%%


