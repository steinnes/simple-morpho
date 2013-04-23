#include "expressions.h"
#include "accounting.h"
extern int DEBUG;
extern int label_offset;
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

void ELiteral::EmitAcc(ostream &o)
{
	o << "(MakeVal " << val << ")" << endl;
}

void EIf::EmitAcc(ostream &o)
{
	// XXX: finna rétt label offset?
	cnd->EmitAcc(o);
	int my_label = label_offset;
	label_offset += 1;
	o << "(GoFalse _" << my_label << ")" << endl;
	stmts->EmitAcc(o);
	o << "_" << my_label << ":" << endl;
}

void EWhile::EmitAcc(ostream &o)
{
	// XXX: finna rétt label offset?
	int my_label = label_offset;
	label_offset += 2;
	o << "_" << my_label << ":" << endl;
	cnd->EmitAcc(o);
	o << "(GoFalse _" << my_label+1 << ")" << endl;
	stmts->EmitAcc(o);
	o << "(Go _" << my_label << ")" << endl;
	o << "_" << my_label+1 << ":" << endl;
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

void EOr::EmitAcc(ostream &o) { }
void EAnd::EmitAcc(ostream &o) { }
void ENot::EmitAcc(ostream &o) { }

void EReturn::EmitAcc(ostream &o)
{
	e->EmitAcc(o);
	o << "(Return 0)" << endl;
}

void EAssign::EmitAcc(ostream &o)
{
	value->EmitAcc(o);
	o << "(Store 0 " << index << ")" << endl;
}
