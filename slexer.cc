#include "slexer.h"
#include <iostream>

extern int DEBUG;

SLexer::SLexer()
{
	l = new yyFlexLexer();
	q = std::queue<Token>();
}

Token SLexer::mkToken()
{
	Token t;
	t.token = l->yylex();
	t.lexeme = (char *)l->YYText();
	t.lineno = l->lineno();
	return t;
}
Token SLexer::advance()
{
	if (q.size())
	{
		Token t = q.front();
		q.pop();
		return t;
	}
	return mkToken();
}

bool SLexer::over(int token)
{
	Token t;
	if (!q.size())
		q.push(mkToken());
	t = q.front();
	if (t.token != token)
		return false;

	q.pop();
	return true;
}

bool SLexer::match(int token)
{
	Token t;
	if (!q.size())
		q.push(mkToken());
	t = q.front();
	if (t.token != token)
		return false;
	return true;
}

Token SLexer::peek()
{
	Token t = mkToken();
	q.push(t);
	return t;
}

