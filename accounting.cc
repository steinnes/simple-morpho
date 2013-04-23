#include "accounting.h"
#include "errors.h"

Accounting::Accounting()
{
	ar = -1;
	call_pos = 0;
	cur_index = 0;
}

bool Accounting::AddVar(string id, Expression *e)
{
	//cout << "AddVar(" << id << ")" << endl;
	//printall();
	if (vars.find(id) != vars.end())
		return false;
	//cout << id << " was not found in vars" << endl;

	if (e != NULL)
		e = new EAssign(cur_index, e);
	Var x;
	x.index = cur_index;
	x.name = id;
	x.expr = e;
	vars[id] = x;

	cur_index += 1;
	return true;
}

void Accounting::printall()
{
	for (map<string, Var>::const_iterator it=vars.begin(); it != vars.end(); it++)
	{
		string n = it->first;
		Var v = it->second;
		cout << n << "[" << v.index << "] " << endl;
	}

}

Var Accounting::GetVar(string id)
{
	//cout << "GetVar(" << id << ")" << endl;
	if (vars.find(id) == vars.end())
		throw VarNotFoundError(id);
	return vars[id];
}

void Accounting::EmitAcc(ostream &o)
{
	for (map<string, Var>::const_iterator it=vars.begin(); it != vars.end(); it++)
	{
		Var v = it->second;
		//cout << "Accounting emit:" << it->first << endl;
		Expression *e = v.expr;
		if (e != NULL)
		{
			//cout << "...=> " << e->type << endl;
			e->EmitAcc(o);
			o << "(Store " << v.index << ")" << endl;
		}
	}	
}

Accounting::~Accounting()
{
	for (map<string, Var>::const_iterator it=vars.begin(); it != vars.end(); it++)
	{
		Expression *e = (Expression *)(it->second.expr);
		if (e != NULL)
			delete e;
	}
}

