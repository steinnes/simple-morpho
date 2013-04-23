#include <iostream>
#include <sstream>
#include "slexer.h"
#include "errors.h"

template <class T> inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

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
	t.lexeme = l->YYText();
	t.lineno = l->lineno();
	return t;
}
Token SLexer::advance()
{
	if (DEBUG) fprintf(stderr, "SLexer::advance():");
	Token t;
	if (q.size())
	{
		t = q.front();
		q.pop();
	}
	else
		t =mkToken();
	if (DEBUG) fprintf(stderr, " %d %s\n", t.token, t.lexeme.c_str());
	return t;
}

void SLexer::skip()
{
	if (q.size())
		q.pop();
	else
		mkToken();
}

bool SLexer::over(int token)
{
	Token t;
	if (!q.size())
		q.push(mkToken());
	t = q.front();
	if (t.token != token)
	{
		string errstr = "Invalid token! Expected: " + to_string(token);
		throw ParseError(t, errstr.c_str());
	}

	q.pop();
	return true;
}

bool SLexer::match(int token)
{
	Token t = peek();
	if (t.token != token)
		return false;
	return true;
}

Token SLexer::peek()
{
	if (!q.size())
		q.push(mkToken());

	return q.front();
}

bool SLexer::eof()
{
	return (peek().token == 0);
}

