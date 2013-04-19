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

F -> L Fx
Fx -> + L Fx
Fx -> - L Fx
Fx -> eps
L -> T Lx
Lx -> * T Lx
Lx -> eps
T -> tala 
T ->'(' F ')'

*/
Expression *F(SLexer *l);


// T -> tala 
// T -> '(' F ')'
Expression *T(SLexer *l)
{
	int token = l->advance();
	if (token == INT)
	{
		int num = atoi(l->text());
		return new Integer(num);
	}
	if (token != LPAREN)
	{
		char err[256];
		sprintf(err, "Expected LPAREN, got: %d (%s)\n\0", token, l->text());
		error(err);
	}
	Expression *op = F(l);
	token = l->advance();
	if (token != RPAREN)
	{
		char err[256];
		sprintf(err, "Expected RPAREN, got: %d (%s)\n\0", token, l->text());
		error(err);
	}
	return op;
}

// Lx -> * T Lx
// Lx -> eps
Expression *Lx(SLexer *l)
{
	int token = l->peek();
	if (token != OP)
	{
		if (DEBUG) cout << "Lx() -> got token=" << token << " (" << l->text() << ")" << endl;
		return T(l);
	}
	
}

// L -> T Lx
Expression *L(SLexer *l)
{
	Expression *a = T(l);
	int token = l->peek();
	if (token != OP)
		return a;

	char op = l->text()[0];
	if (op != '*')
	{
		if (DEBUG) cout << "token != OP (" << token << ") op='" << op << "'" << endl;
		return a;
	}
	l->over(); // burn off the peeked op
	return new Binary('*', a, Lx(l));
}

// Fx -> + L Fx
// Fx -> - L Fx
// Fx -> eps
Expression *Fx(SLexer *l)
{
	Expression *a = L(l);

	int token = l->peek();
	if (token != OP)
		return a;
	char op = l->text()[0];
	if (op=='+'||op=='-')
	{
		l->over(); // burn off peeked op
		return new Binary(op, a, Fx(l));
	}
	return NULL;
}


// F -> L Fx
Expression *F(SLexer *l)
{
	Expression *a = L(l); // L

	int token = l->peek();
	if (token != OP)
	{
		if (DEBUG) cout << "peeked token=" << token << " (" << l->text() << ")" << endl;
		return a;
	}

	
	char op = l->text()[0];
	l->over(); // burn off the peeked token, we have it in "op"
	if (DEBUG) cout << "F .. op=" << op << endl;

	Expression *b = Fx(l); // Fx
	if (DEBUG) cout << "F .. b=" << b->compute() << endl;

	switch (op)
	{
		case '+':
			return new Binary('+', a, b);
		case '-':
			return new Binary('-', a, b);
		default:
			char err[256];
			sprintf(err, "Unsupported OP, got: %d (%s)\n\0", token, l->text());
			error(err);
		break;
	}
	return NULL;
}

int main(void)
{
	SLexer *lexer = new SLexer;
	Expression *e = F(lexer);
	cout << e->compute() << endl;
}
