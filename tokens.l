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
OP 		[+\-\*]|"=="
NEWLINE 	\r|\n|\r\n
WHITESPACE      [\n\r\ \t\b\012]

%%
{INT} 		{ return(INT); }
{FLOAT} 	{ return(FLOAT); }
{STRING} 	{ return(STRING); }
{TRUE} 		{ return(TRUE); }
{FALSE} 	{ return(FALSE); }
{OP} 		{ return(OP); }
"if"		{ return(IF); }
"while"		{ return(WHILE); }
{NEWLINE} 	{ line++; }
{WHITESPACE} 	{ }  // skip whitespaces
"{"		{ return(LBRACE); }
"}"		{ return(RBRACE); }
","		{ return(COMMA); }
";"		{ return(SEMICOLON); }
"("		{ return(LPAREN); }
")"		{ return(RPAREN); }

%%


