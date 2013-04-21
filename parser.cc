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

class EIf : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EIf(Expression *e, ExprList *el) : cnd(e), stmts(el) { };
};

class EWhile : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EWhile(Expression *e, ExprList *el) : cnd(e), stmts(el) { };
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

ExprList *body(SLexer *);
Expression *expr(SLexer *);

Expression *small_expr(SLexer *l)
{
	Token token = l->peek();
	cout << "small_expr() .. peeked token = " << token.token <<  "("  << token.lexeme << ")" << endl;

	Expression *cnd;
	ExprList *stmts;
	switch (token.token)
	{
		case STRING:
		case FLOAT:
		case INT:
		case FALSE:
		case TRUE:
			// expecting assignment ?
			cout << "Returning new ELiteral(" << token.lexeme << ")" << endl;
			l->skip();
			return new ELiteral(token.lexeme);
			break;
		case IF:
			l->skip();

			l->over(LPAREN);
			cnd = expr(l);
			l->over(RPAREN);
			stmts = body(l);
			return new EIf(cnd, stmts);
		case WHILE:
			l->skip();
			l->over(LPAREN);
			cnd = expr(l);
			l->over(RPAREN);
			stmts = body(l);
			return new EWhile(cnd, stmts);
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
