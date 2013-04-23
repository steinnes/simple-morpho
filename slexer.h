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
	string lexeme;
} Token;

class ParseError : public exception
{
protected:
	char buf[512];
public:
	ParseError() { };
	ParseError(Token t, const char *msg)
	{
		sprintf(buf, "Parse Error: %s\n\tline %d, token %d, lexeme '%s'", msg, t.lineno, t.token, t.lexeme.c_str());
	}
	virtual const char* what() const throw()
	{
		return buf;
	}
};

class OperatorError : public ParseError
{
public:
	OperatorError(string msg)
	{
		sprintf(buf, "Operator Error: %s\n", msg.c_str());
	};
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
	void skip();
	bool over(int token);
	bool match(int token);
	bool eof();
};

#endif
