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

class SLexer
{
private:
	FlexLexer *l;
	deque<struct Token> q;
	Token mkToken();
public:
	int last;
	SLexer();
	Token advance();
	Token peek(int);
	Token peek();
	int line();
	void skip();
	bool over(int token);
	bool match(int token);
	bool eof();
};

#endif
