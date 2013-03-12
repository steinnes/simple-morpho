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

	int compute()
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

int main(void)
{
	FlexLexer *lexer = new yyFlexLexer;
	token = lexer->yylex();
}
