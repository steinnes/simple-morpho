#include "expressions.h"
#include "accounting.h"
extern int DEBUG;
extern Accounting *acc;

void Expression::EmitArg(ostream &o, int ar, int varpos)
{
	EmitAcc(o);
	o << "(StoreArgAcc " << ar << " " << varpos << ")" << endl;
}

void ExprList::Add(Expression *e) { if (DEBUG) { cerr << "Adding " << e->type << " to ExprList" << endl; } expr_list.push_back(e); }
void ExprList::Add(ExprList *e)
{
	vector<Expression *> el = e->expr_list;
	for (vector<Expression *>::const_iterator it=el.begin(); it!=el.end(); it++)
		expr_list.push_back(*it);
}
void ExprList::EmitAcc(ostream &o)
{
	for (vector<Expression *>::const_iterator it=expr_list.begin(); it != expr_list.end(); it++)
	{
		Expression *e = *it;
		e->EmitAcc(o);
	}
}
void ExprList::EmitArgs(ostream &o, int AR)
{
	int i = 0;
	for (vector<Expression *>::const_iterator it=expr_list.begin(); it != expr_list.end(); it++)
	{
		Expression *e = *it;
		e->EmitArg(o, AR, i);
		i++;
	}
}

Expression *ExprList::last()
{
	if (expr_list.begin() != expr_list.end())
	{
		return *(expr_list.end());
	}
	else
		return NULL;
}
int ExprList::n()
{
	return expr_list.size();
}

void ELiteral::EmitAcc(ostream &o)
{
	o << "(MakeVal " << val << ")" << endl;
}

void EWhile::EmitAcc(ostream &o)
{
	// XXX: finna rétt label offset?
	int start = acc->NewLabel();
	int end = acc->NewLabel();
	o << "_" << start << ":" << endl;
	cnd->EmitAcc(o);
	o << "(GoFalse _" << end << ")" << endl;
	stmts->EmitAcc(o);
	o << "(Go _" << start << ")" << endl;
	o << "_" << end << ":" << endl;
}

void ECall::EmitAcc(ostream &o)
{
	int AR = acc->ar;
	acc->ar -= 1;
	args->EmitArgs(o, AR);
	o << "(Call #\"" << function << "[f" << args->expr_list.size() << "]\" " << AR << ")" << endl;
}

void EVar::EmitAcc(ostream &o)
{
	Var v = acc->GetVar(name);
	o << "(Fetch " << v.index << ")" << endl;
}

void EBinOp::EmitAcc(ostream &o)
{
	int AR = acc->ar;
	acc->ar -= 1;
	a->EmitArg(o, AR, 0);
	b->EmitArg(o, AR, 1);
	o << "(Call #\"" << op << "[f2]\" " << AR << ")" << endl;
}

void EOr::EmitAcc(ostream &o)
{
	int labtrue = acc->NewLabel();
	a->EmitAcc(o);
	o << "(GoTrue _" << labtrue << ")" << endl;
	b->EmitAcc(o);
	o << "_" << labtrue << ":" << endl;
}

void EIf::EmitAcc(ostream &o)
{
	// XXX: finna rétt label offset?
	cnd->EmitAcc(o);
	int my_label = acc->NewLabel();
	o << "(GoFalse _" << my_label << ")" << endl;
	stmts->EmitAcc(o);
	o << "_" << my_label << ":" << endl;
}

void EAnd::EmitAcc(ostream &o)
{
	int labfalse = acc->NewLabel();
	a->EmitAcc(o);
	o << "(GoFalse _" << labfalse << ")" << endl;
	b->EmitAcc(o);
	o << "_" << labfalse << ":" << endl;
}

void ENot::EmitAcc(ostream &o) { }

void EReturn::EmitAcc(ostream &o)
{
	if (e != NULL)
			e->EmitAcc(o);
	o << "(Return 0)" << endl;
}

void EAssign::EmitAcc(ostream &o)
{
	value->EmitAcc(o);
	o << "(Store 0 " << index << ")" << endl;
}
