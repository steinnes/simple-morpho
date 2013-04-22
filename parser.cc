#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tokens.h"
#include "slexer.h"

using namespace std;
int token;

int DEBUG = 1;


// Bókhald fyrir hvert fall
typedef struct Var
{
	int index;
	string name;
} Var;

class Accounting
{
private:
	int cur_index;
	map<string, Var> vars;
public:
	Accounting()
	{
		cur_index = 0;
	};
	bool AddVar(string id)
	{
		if (vars.find(id) != vars.end())
			return false;
		Var x;
		x.index = cur_index++;
		x.name = id;
		vars[id] = x;
		return true;
	};
	Var GetVar(string id)
	{
		return vars[id];
	};
};

Accounting *acc;

class Expression
{
public:
	string type;
	virtual ~Expression() {};
	virtual void EmitAcc(ostream&) {};
};

class ExprList : public Expression
{
public:
	ExprList() { type = "ExprList"; }
	vector<Expression *> expr_list;
	void Add(Expression *e) { if (DEBUG) { cerr << "Adding " << e->type << " to ExprList" << endl; } expr_list.push_back(e); };
	void Add(ExprList *e)
	{
		vector<Expression *> el = e->expr_list;
		for (vector<Expression *>::const_iterator it=el.begin(); it!=el.end(); it++)
			expr_list.push_back(*it);
	};
	void EmitAcc(ostream &o)
	{
		for (vector<Expression *>::const_iterator it=expr_list.begin(); it != expr_list.end(); it++)
		{
			Expression *e = *it;
			e->EmitAcc(o);
		}
	};

};

class ELiteral : public Expression
{
public:
	string val;
	ELiteral(string &value) : val(value) { type = "ELiteral"; };
	ELiteral(char *value) : val(value) { type = "ELiteral"; };
	void EmitAcc(ostream &o)
	{
		o << "(MakeVal " << val << ")" << endl;
	};
};

class EIf : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EIf(Expression *e, ExprList *el) : cnd(e), stmts(el) { type = "EIf"; };
	void EmitAcc(ostream &o)
	{
		// XXX: finna rétt label offset?
		int label_offset = 0;
		cnd->EmitAcc(o);
		o << "(GoFalse _" << label_offset << ")" << endl;
		stmts->EmitAcc(o);
		o << "_" << label_offset << ":" << endl;
	};
};

class EWhile : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EWhile(Expression *e, ExprList *el) : cnd(e), stmts(el) { type = "EWhile"; };
	void EmitAcc(ostream &o)
	{
		// XXX: finna rétt label offset?
		int label_offset = 0;
		o << "_" << label_offset << ":" << endl;
		cnd->EmitAcc(o);
		o << "(GoFalse _" << label_offset+1 << ")" << endl;
		stmts->EmitAcc(o);
		o << "(Go _" << label_offset << ")" << endl;
		o << "_" << label_offset+1 << ":" << endl;
	};
};

class EVar : public Expression
{
private:
	string name;
public:
        EVar(string &n) : name(n) { type = "EVar"; };
        EVar(char *n) : name(n) { type = "EVar"; };
};

class EBinOp : public Expression
{
private:
	Expression *a;
	Expression *b;
	string op;
public:
	EBinOp(string &o, Expression *ae, Expression *be) : op(o), a(ae), b(be) { type = "EBinOp"; };
	void EmitAcc(ostream &o)
	{
		a->EmitAcc(o);
		o << "(StoreArgAcc -1 0)" << endl; // þurfum ekki að spá í -1 því við köllum strax í þetta?
		b->EmitAcc(o);
		o << "(StoreArgAcc -1 1)" << endl;
		o << "(Call #\"" << op << "f[2]\" -1)" << endl;
	};
};

class EOr : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EOr(Expression *ae, Expression *be) : a(ae), b(be) { type = "EOr"; };
};

class EAnd : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EAnd(Expression *ae, Expression *be) : a(ae), b(be) { type = "EAnd"; };
};

class ENot : public Expression
{
private:
	Expression *e;
public:
	ENot(Expression *expr) : e(expr) { type = "ENot"; };
};

class EReturn : public Expression
{
private:
	Expression *e;
public:
	EReturn(Expression *expr) : e(expr) { type = "EReturn"; };
	void EmitAcc(ostream &o)
	{
		e->EmitAcc(o);
		o << "(Return 0)" << endl;
	};
};

class EAssign : public Expression
{
private:
	string id;
	Expression *value;
public:
	EAssign(string varname, Expression *e) : id(varname), value(e) { type = "EDeclAssign"; };
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
	if (DEBUG) cerr << "small_expr() .. peeked token = " << token.token <<  "("  << token.lexeme << ")" << endl;

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
			if (DEBUG) cerr << "Returning new ELiteral(" << token.lexeme << ")" << endl;
			l->skip();
			ELiteral *e;
			e = new ELiteral(token.lexeme);
			if (DEBUG) fprintf(stderr, "About to return new ELiteral, address: %p val=%s\n", e, e->val.c_str());
			return e;
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

	t = l->peek();
	opname = t.lexeme;

	if (priority(opname) == p)
		return expr;
	l->advance();

	// XXX: sleppi if (p==2)
	while (true)
	{
		expr = new EBinOp(opname, expr, binop_expr(l, p+1));
		if (!l->match(OP))
			return expr;

		t = l->peek();
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
	return a;
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

// þetta fall er ekki rétt..
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
			el->Add(new EAssign(t.lexeme, expr(l))); // XXX: ????
		}
		else if (decltok == VAR)
		{
			el->Add(new EDecl(t.lexeme));
		}
		else
			throw ParseError(t, "Parsing declaration, expected VAR/VAL");
		if (l->match(COMMA)) // XXX: messy?
			l->skip();
	}
	l->over(SEMICOLON);
	return el;
}


ExprList *body(SLexer *l)
{
	ExprList *el = new ExprList();
	l->over(LBRACE);

	while (l->match(VAR) || l->match(VAL))
	{
		el->Add(decls(l));
	}

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
	acc = new Accounting();
	try
	{
		// XXX: replace body() here with program()->function()->body()...
		ExprList *el = body(lexer);
		cout << "\"einfalt.mexe\" = main in" << endl << "!" << endl << "{{" << endl << "#\"main[f0]\" =" << endl << "[" << endl;
		el->EmitAcc(cout);
		cout << "];" << endl << "}}" << "*" << endl << "BASIS" << endl << ";" << endl;
	}
	catch (ParseError e)
	{
		cout << e.what() << endl;
	}
/*	Expression *e = F(lexer);
	cout << e->compute() << endl;*/
}
