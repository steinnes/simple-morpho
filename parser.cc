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
public:
	vector<Expression *> expr_list;
	void Add(Expression *e) { expr_list.push_back(e); };
	void Add(ExprList *e) {
		vector<Expression *> el = e->expr_list;
		for (vector<Expression *>::const_iterator it=el.begin(); it!=el.end(); it++)
			expr_list.push_back(*it);
	};
};

class ELiteral : public Expression
{
private:
	string val;
public:
	ELiteral(string &value) : val(value) {};
	ELiteral(char *value) : val(value) {};
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

class EVar : public Expression
{
private:
	string name;
public:
        EVar(string &n) : name(n) {};
        EVar(char *n) : name(n) {};
};

class EBinOp : public Expression
{
private:
	Expression *a;
	Expression *b;
	string op;
public:
	EBinOp(string &o, Expression *ae, Expression *be) : op(o), a(ae), b(be) {};
};

class EOr : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EOr(Expression *ae, Expression *be) : a(ae), b(be) {};
};

class EAnd : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EAnd(Expression *ae, Expression *be) : a(ae), b(be) {};
};

class ENot : public Expression
{
private:
	Expression *e;
public:
	ENot(Expression *expr) : e(expr) { };
};

class EReturn : public Expression
{
private:
	Expression *e;
public:
	EReturn(Expression *expr) : e(expr) { };
};

// decls: ... XXX: should EDecl not exist, and use EVar instead?
class EDecl : public Expression
{
private:
	string id;
public:
	EDecl(string varname) : id(varname) {};
};

class EDeclAssign : public Expression
{
private:
	string id;
	Expression *value;
public:
	EDeclAssign(string varname, Expression *e) : id(varname), value(e) {};
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
Expression *or_expr(SLexer *);
Expression *and_expr(SLexer *);
Expression *not_expr(SLexer *);

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
		case ID:
			l->skip();
			return new EVar(token.lexeme);
		default:
		break;
	}
}

int priority(const string &s)
{
	switch (s[0])
	{
		case '?': return(1);
		case '~': return(1);
		case '^': return(1);
		case ':': return(2);
		case '|': return(3);
		case '&': return(4);
		case '=': return(5);
		case '!': return(5);
		case '>': return(5);
		case '<': return(5);
		case '+': return(6);
		case '-': return(6);
		case '*': return(7);
		case '/': return(7);
		case '%': return(7);
	}
	throw OperatorError("Unknown operator:" + s);
}

Expression *fun_expr(SLexer *l)
{
	return small_expr(l);
}

Expression *binop_expr(SLexer *l, int p)
{
	string opname;
	Expression *expr;
	Token t;
	if (p == 8)
		return fun_expr(l);

	expr = binop_expr(l, p+1);
	if (!l->match(OP))
		return expr;

	t = l->advance();
	opname = t.lexeme;

	if (priority(opname) == p)
		return expr;
	l->advance();

	// XXX: sleppi if (p==2)
	while (true)
	{
		expr = new EBinOp(opname, expr, binop_expr(l, p+1));
		if (!l->match(OP))
		{
			cout << "binop_expr() returning expression, op=" << opname << endl;
			return expr;
		}

		t = l->advance();
		opname = t.lexeme;

		if (priority(opname) == p)
			return expr;
		l->advance();
	}
}

Expression *expr(SLexer *l)
{
	Token t = l->peek();
	if (t.token == RETURN)
	{
		l->skip();
		return new EReturn(expr(l));
	}
	// XXX: implement <id> = <expr>
	return or_expr(l);
}
Expression *or_expr(SLexer *l)
{
	Expression *a = and_expr(l);
	Token t = l->peek();
	if (t.token == OR)
	{
		l->skip();
		return new EOr(a, or_expr(l));
	}
	return a;
}
Expression *and_expr(SLexer *l)
{
	Expression *a = not_expr(l);
	Token t = l->peek();
	if (t.token == AND)
	{
		l->skip();
		return new EAnd(a, and_expr(l));
	}
	return not_expr(l);
}
Expression *not_expr(SLexer *l)
{
	Token t = l->peek();
	if (t.token == NOT)
	{
		l->skip();
		return new ENot(not_expr(l));
	}
	return binop_expr(l, 1);
}

ExprList *decls(SLexer *l)
{
	ExprList *el = new ExprList();
	Token t = l->peek();
	int decltok = t.token;
	l->skip(); // skip over decltok
	while (!l->match(SEMICOLON))
	{
		t = l->advance();
		if (decltok == VAL)
		{
			l->over(OP); // XXX: should be new token EQ ?
			el->Add(new EDeclAssign(t.lexeme, expr(l))); // XXX: ????
		}
		else if (decltok == VAR)
		{
			el->Add(new EDecl(t.lexeme));
		}
		if (l->match(COMMA)) // XXX: messy?
			l->skip();
	}
	throw ParseError(t, "Parsing declaration, expected VAR/VAL");
}


ExprList *body(SLexer *l)
{
	ExprList *el = new ExprList();
	l->over(LBRACE);

	while (l->match(VAR) || l->match(VAL))
		el->Add(decls(l));

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
