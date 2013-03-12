#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <FlexLexer.h>

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
	int compute()	{ return val; }
	void print()	{ std::cout << val; }
};

int main(void)
{
	FlexLexer *lexer = new yyFlexLexer;
	do
	{
		token = lexer->yylex();
		cout << "Line: " << lexer->lineno() << " token: " << token << endl;
		// errrr ... F(c) ? .. svo T(c) ? svo hums
	}
	while(token != 0);
}
