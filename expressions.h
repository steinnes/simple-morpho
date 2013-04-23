#ifndef __EXPRESSIONS_H__
#define __EXPRESSIONS_H__
#include <string>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

class Expression
{
public:
	string type;
	virtual ~Expression() {};
	virtual void EmitAcc(ostream &o);
};

class ExprList : public Expression
{
public:
	ExprList() { type = "ExprList"; }
	vector<Expression *> expr_list;
	void Add(Expression *e);
	void Add(ExprList *e);
	void EmitAcc(ostream &o);
};

class ELiteral : public Expression
{
public:
	string val;
	ELiteral(string &value) : val(value) { type = "ELiteral"; };
	ELiteral(char *value) : val(value) { type = "ELiteral"; };
	void EmitAcc(ostream &o);
};

class EIf : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EIf(Expression *e, ExprList *el) : cnd(e), stmts(el) { type = "EIf"; };
	void EmitAcc(ostream &o);
};

class EWhile : public Expression
{
private:
	Expression *cnd;
	ExprList *stmts;
public:
	EWhile(Expression *e, ExprList *el) : cnd(e), stmts(el) { type = "EWhile"; };
	void EmitAcc(ostream &o);
};

class ECall : public Expression
{
private:
	string function;
public:
	void EmitAcc(ostream &o);
};

class EVar : public Expression
{
private:
	string name;
public:
	EVar(string &n) : name(n) { type = "EVar"; };
	EVar(char *n) : name(n) { type = "EVar"; };
	void EmitAcc(ostream &o);
};

class EBinOp : public Expression
{
private:
	Expression *a;
	Expression *b;
	string op;
public:
	EBinOp(string &o, Expression *ae, Expression *be) : op(o), a(ae), b(be) { type = "EBinOp"; };
	void EmitAcc(ostream &o);
};

class EOr : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EOr(Expression *ae, Expression *be) : a(ae), b(be) { type = "EOr"; };
	void EmitAcc(ostream &o);
};

class EAnd : public Expression
{
private:
	Expression *a;
	Expression *b;
public:
	EAnd(Expression *ae, Expression *be) : a(ae), b(be) { type = "EAnd"; };
	void EmitAcc(ostream &o);
};

class ENot : public Expression
{
private:
	Expression *e;
public:
	ENot(Expression *expr) : e(expr) { type = "ENot"; };
	void EmitAcc(ostream &o);
};

class EReturn : public Expression
{
private:
	Expression *e;
public:
	EReturn(Expression *expr) : e(expr) { type = "EReturn"; };
	void EmitAcc(ostream &o);
};

class EAssign : public Expression
{
private:
	int index;
	Expression *value;
public:
	EAssign(int idx, Expression *e) : index(idx), value(e) { type = "EAssign"; };
	void EmitAcc(ostream &o);
};

#endif
