#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tokens.h"
#include "slexer.h"

using namespace std;
int token;

int DEBUG = 0;

class Expression
{
public:
	virtual ~Expression() {}
};

class ELiteral : public Expression
{
private:
	string val;
public:
	ELiteral(string &value) : val(value) {};
	ELiteral(char *value) : val(value) {}
};

class Integer : public Expression
{
private:
	int val;
	Integer();
public:
	Integer(int val)
	{
		this->val = val;
	}
	int compute() const { return val; }
	void print()	{ std::cout << val; }
};

void error(char *errstr)
{
	cerr << errstr << endl;
	exit(1);
}

void printToken(Token t)
{
	cout << "line: "<< t.lineno << " token: " << t.token << " (" << t.lexeme << ")" << endl;
}

Expression *small_expr(SLexer *l)
{
	Token token = l->peek();
	switch (token.token)
	{
		case STRING:
		case FLOAT:
		case INT:
		case FALSE:
		case TRUE:
			// expecting assignment ?
			return new ELiteral(token.lexeme);
			break;
		default:
		break;
	}

	while (!l->match(SEMICOLON))
		printToken(l->advance());
}

Expression *expr(SLexer *l)
{
	return small_expr(l);
}

void body(SLexer *l)
{
	l->over(LBRACE);
	while (!l->match(RBRACE))
	{
		expr(l);
		l->over(SEMICOLON);
	}
	l->over(RBRACE);
}

int main(void)
{
	SLexer *lexer = new SLexer;
	try
	{
		body(lexer);
	}
	catch (ParseError e)
	{
		cout << e.what() << endl;
	}
/*	Expression *e = F(lexer);
	cout << e->compute() << endl;*/
}
