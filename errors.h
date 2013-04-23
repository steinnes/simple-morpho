#ifndef __ERRORS_H__
#define __ERRORS_H__

#include "slexer.h" // for Token

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

class VarNotFoundError : public ParseError
{
public:
	VarNotFoundError(string id)
	{
		Token t;
		t.lineno = -1;
		t.token = -1;
		t.lexeme = id;
		ParseError(t, "Variable not found");
	};
};

class OperatorError : public ParseError
{
public:
	OperatorError(string msg)
	{
		sprintf(buf, "Operator Error: %s\n", msg.c_str());
	};
};

#endif
