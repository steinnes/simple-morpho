#ifndef __SLEXER_H__
#define __SLEXER_H__

#include <FlexLexer.h>
#include <string.h>
#include <queue>

using namespace std;

typedef struct Token
{
	int token;
	int lineno;
	char *lexeme;
} Token;

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
