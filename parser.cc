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
			case '-':
				return left->compute() - right->compute();
				break;
			case '+':
				return left->compute() + right->compute();
				break;
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
F -> T + T
F -> T - T
T -> tala | '(' F ')'

*/

Integer *parseInt(FlexLexer *l)
{
	int token = l->yylex();
	if (token != INT)
	{
		char err[256];
		sprintf(err, "Expected INT, got: %d (%s)\n\0", token, l->YYText());
		error(err);
	}
	int num = atoi(l->YYText());
	return new Integer(num);
}

Binary *parseBinOp(FlexLexer *l)
{
	Integer *a = parseInt(l);
	int token = l->yylex();
	if (token != OP)
	{
		char err[256];
		sprintf(err, "Expected OP, got: %d (%s)\n\0", token, l->YYText());
		error(err);
	}

	char op = l->YYText()[0];

	Integer *b = parseInt(l);

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
	Expression *e = parseBinOp(lexer);
	cout << e->compute() << endl;
}
