#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <FlexLexer.h>
#include "tokens.h"

using namespace std;
int token;

class Expression
{
public:
	virtual ~Expression() {}
	virtual int compute() const=0;
	virtual void print()=0;
};

class Binary : public Expression
{
private:
	char op;
	Expression *left;
	Expression *right;
	Binary();
public:
	Binary(char op, Expression *left, Expression *right)
	{
		this->op = op;
		this->left = left;
		this->right = right;
	}

	int compute() const
	{
		switch(op)
		{
			case '*':
				return left->compute() * right->compute();
			case '-':
				return left->compute() - right->compute();
			case '+':
				return left->compute() + right->compute();
			default:
				std::cout << "unknown op:" << op << std::endl;
		}
		return 0;
	}
	void print()
	{
		std::cout << "left=";
		left->print();
		std::cout << "op=" << op << " right=";
		right->print();
		std::cout << std::endl;
	}
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

/* grammar:

F -> L | L + L | L - L
L -> T | T * T
T -> tala | '(' F ')'

F = parseFormula
L = parseLumma
T = parseTerm


*/
Binary *parseFormula(FlexLexer *l);

Expression *parseTerm(FlexLexer *l)
{
	int token = l->yylex();
	if (token == INT)
	{
		int num = atoi(l->YYText());
		return new Integer(num);
	}
	if (token != LPAREN)
	{
		char err[256];
		sprintf(err, "Expected LPAREN, got: %d (%s)\n\0", token, l->YYText());
		error(err);
	}
	Binary *op = parseFormula(l);
	token = l->yylex();
	if (token != RPAREN)
	{
		char err[256];
		sprintf(err, "Expected RPAREN, got: %d (%s)\n\0", token, l->YYText());
		error(err);
	}
	return op;
}

Expression *parseLumma(FlexLexer *l)
{
	int token = l->yylex();
	// þyrfti að peek-a hér..
	
}

Binary *parseFormula(FlexLexer *l)
{
	Expression *a = parseTerm(l);
	int token = l->yylex();
	if (token != OP)
	{
		char err[256];
		sprintf(err, "Expected OP, got: %d (%s)\n\0", token, l->YYText());
		error(err);
	}

	char op = l->YYText()[0];

	Expression *b = parseTerm(l);

	switch (op)
	{
		case '+':
			return new Binary('+', a, b);
		case '-':
			return new Binary('-', a, b);
		default:
			char err[256];
			sprintf(err, "Unsupported OP, got: %d (%s)\n\0", token, l->YYText());
			error(err);
		break;
	}
	return NULL;
}

int main(void)
{
	FlexLexer *lexer = new yyFlexLexer;
	Expression *e = parseFormula(lexer);
	cout << e->compute() << endl;
}
