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

class ExprList : public Expression
{
private:
	std::vector<Expression *> expr_list;
public:
	void Add(Expression *e) { expr_list.push_back(e); };
};

class ELiteral : public Expression
{
private:
	string val;
public:
	ELiteral(string &value) : val(value) {};
	ELiteral(char *value) : val(value) {}
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
	cout << "small_expr() .. peeked token = " << token.token <<  "("  << token.lexeme << ")" << endl;
	switch (token.token)
	{
		case STRING:
		case FLOAT:
		case INT:
		case FALSE:
		case TRUE:
			// expecting assignment ?
			cout << "Returning new ELiteral(" << token.lexeme << ")" << endl;
			l->over(token.token); // XXX: rewrite and use "skip" function
			return new ELiteral(token.lexeme);
			break;
		case IF:
		default:
		break;
	}

/*
	while (!l->match(SEMICOLON))
		printToken(l->advance());
*/
}

Expression *expr(SLexer *l)
{
	return small_expr(l);
}

ExprList *body(SLexer *l)
{
	ExprList *el = new ExprList();
	l->over(LBRACE);
	while (!l->match(RBRACE))
	{
		el->Add(expr(l));
		l->over(SEMICOLON);
	}
	l->over(RBRACE);
	return el;
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
