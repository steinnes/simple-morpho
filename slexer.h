#ifndef __SLEXER_H__
#define __SLEXER_H__

#include <FlexLexer.h>
#include <string.h>
#include <queue>
#include <stdio.h>

using namespace std;

typedef struct Token
{
	int token;
	int lineno;
	char *lexeme;
} Token;

class ParseError : exception
{
private:
	char buf[512];
public:
	ParseError(Token t, const char *msg)
	{
		sprintf(buf, "Parse Error: %s\n\tline %d, token %d, lexeme '%s'", msg, t.lineno, t.token, t.lexeme);
	}
	virtual const char* what() const throw()
	{
		return buf;
	}
};

class SLexer
{
private:
	FlexLexer *l;
	queue<struct Token> q;
	Token mkToken();
public:
	SLexer();
	Token advance();
	Token peek();
	int line();
	bool over(int token);
	bool match(int token);
};

#endif
