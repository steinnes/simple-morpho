#include "expressions.h"
#include "accounting.h"
extern int DEBUG;
extern int label_offset;
extern Accounting *acc;

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
}

void EVar::EmitAcc(ostream &o)
{
	Var v = acc->GetVar(name);
	o << "(Fetch " << v.index << ")" << endl;
}

void EBinOp::EmitAcc(ostream &o)
{
	a->EmitAcc(o);
	o << "(StoreArgAcc -1 0)" << endl; // þurfum ekki að spá í -1 því við köllum strax í þetta?
	b->EmitAcc(o);
	o << "(StoreArgAcc -1 1)" << endl;
	o << "(Call #\"" << op << "[f2]\" -1)" << endl;
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
