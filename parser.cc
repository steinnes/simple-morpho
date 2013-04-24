#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tokens.h"
#include "slexer.h"
#include "expressions.h"
#include "accounting.h"
#include "errors.h"

using namespace std;
int token;

int DEBUG = 0;
Accounting *acc;

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
/*		case '?': return(1);
		case '~': return(1);
		case '^': return(1);
		case ':': return(2);
		case '|': return(3);
		case '&': return(4); */
		case '=': return(5);
		case '!': return(5);
		case '>': return(5);
		case '<': return(5);
		case '+': return(7);
		case '-': return(7);
		case '*': return(6);
		case '/': return(6);
		case '%': return(6);
	}
	throw OperatorError("Unknown operator:" + s);
}

Expression *fun_expr(SLexer *l)
{
	Token t = l->peek();
	if (t.token == ID)
	{
		Token t2 = l->peek(2);
		if (t2.token == LPAREN)
		{
			ExprList *el = new ExprList();
			l->skip(); // skip over ID
			l->skip(); // skip over LPAREN
			while (!l->match(RPAREN))
			{
				el->Add(expr(l));
				if (l->match(COMMA))
					l->skip();
			}
			l->over(RPAREN);
			return new ECall(t.lexeme, el);
		}
	}
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
	Token t = l->peek(1);
	if (t.token == RETURN)
	{
		l->skip();
		return new EReturn(expr(l));
	}
	else if (t.token == ID)
	{
		Token t2 = l->peek(2);
		if (DEBUG)
		{
			cerr << "expr(): found ID" << endl;
			cerr << t2.token << "( " << t2.lexeme << " )" << endl;
		}
		if (t2.token == ASSIGN)
		{
			// 1. finna id position í *acc
			Var v = acc->GetVar(t.lexeme);
			if (DEBUG) cerr << "expr(): found var assignment: " << t.lexeme << endl;
			// 2. returna EAssign(position, <expr>);
			l->skip();
			l->skip();
			if (DEBUG) cerr << "expr(): skipped over ASSIGN token, returning new EAssign(" << v.index << ", expr(l))" << endl;
			return new EAssign(v.index, expr(l));
		}
	}
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
		if (DEBUG) cerr << "Returning new EAnd..." << endl;
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

void *decls(SLexer *l)
{
	Token t = l->advance();
	int decltok = t.token;
	while (!l->match(SEMICOLON))
	{
		t = l->advance();
		//cout << "decls() processing: " << t.lexeme << " (" << t.token << ")" << endl;
		assert(t.token == ID);
		if (decltok == VAL)
		{
			l->over(ASSIGN);
			if (!acc->AddVar(t.lexeme, expr(l)))
				throw ParseError(t, "Variable already defined..");
		}
		else if (decltok == VAR)
		{
			//el->Add(new EDecl(t.lexeme));
			if (!acc->AddVar(t.lexeme, NULL))
				throw ParseError(t, "Variable already defined..");
		}
		else
			throw ParseError(t, "Parsing declaration, expected VAR/VAL");
		if (l->match(COMMA))
			l->skip();
	}
	l->over(SEMICOLON);
}

ExprList *body(SLexer *l)
{
	ExprList *el = new ExprList();
	l->over(LBRACE);

	while (l->match(VAR) || l->match(VAL))
	{
		decls(l);
	}

	while (!l->match(RBRACE))
	{
		el->Add(expr(l));
		l->over(SEMICOLON);
	}
	l->over(RBRACE);
	return el;
}

void function(SLexer *l, ostream &o)
{
	string funcid;
	l->over(FUN);
	if (l->match(ID))
	{
		Token t = l->advance();
		funcid = t.lexeme;
		// parse (id_list)
		l->over(LPAREN);
		while (!l->match(RPAREN))
		{
			Token t = l->advance();
			if (t.token == ID)
			{
				if (!acc->AddVar(t.lexeme, NULL))
					throw ParseError(t, "Invalid function parameter, duplicate?");
			}
			else
				throw ParseError(t, "Invalid function parameter, expected ID");
			if (l->match(COMMA))
				l->skip();
		}
		l->over(RPAREN);
		o << "#\"" << funcid << "[f" << acc->index() << "]\" =" << endl << "[" << endl;
		ExprList *el = body(l);
		// return forced here... should force (MakeVal null) ... XXX
		if (el->n() == 0)
			el->Add(new EReturn(NULL));

		acc->EmitAcc(o);
		el->EmitAcc(o);
		o << "];" << endl;
	}
}

void program(SLexer *l, ostream &o, string progname)
{
	// XXX: hardcoding "main" .. we need a main ;-)
	o << "\"" << progname << ".mexe\" = main in" << endl << "!" << endl << "{{" << endl;
	while (!l->eof())
	{
		acc = new Accounting(); // acc is our function var accounting object
		function(l, o);
		delete acc;
	}
	o << "}}" << " *" << endl << "BASIS" << endl << ";" << endl;
}

int main(int argc, char *argv[])
{
	int c;
	string progname = "default"; // some lame default value

	if (argc < 2)
	{
		cerr << "Running without params, output to stdout, progname=" << progname << endl;
	}
	while ((c = getopt(argc, argv, "o:dh")) != -1)
	{
		switch (c)
		{
			case 'h':
				cout << "Usage: " << endl << argv[0] << " [-d] [-o program] < source.sm " << endl;
				cout << "\t -d :\t enables debugging output to stderr" << endl;
				cout << "\t -o :\t enables debugging output to stderr" << endl;
				return 1;

			case 'o': // -o specified, set progname
				progname = optarg;
			break;
			case 'd':
				DEBUG = 1;
			break;
			default:
			break;
		}
	}
	SLexer *lexer = new SLexer;
	acc = new Accounting();
	try
	{
		if (progname != "default")
		{
			string fname = progname + ".masm";
			ofstream fout (fname.c_str(), std::ofstream::out);
			program(lexer, fout, progname); 
		}
		else
			program(lexer, cout, progname); 
	}
	catch (ParseError e)
	{
		cout << e.what() << endl;
	}
/*	Expression *e = F(lexer);
	cout << e->compute() << endl;*/
}
