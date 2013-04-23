#ifndef __ACCOUNTING_H__
#define __ACCOUNTING_H__
#include <string>
#include <map>
#include "expressions.h"
using namespace std;

class Expression; // 

typedef struct Var
{
	int index;
	string name;
	Expression *expr;
} Var;

class Accounting
{
private:
	int cur_index;
	map<string, Var> vars;
public:
	int ar;
	int call_pos;

	Accounting();
	~Accounting();
	bool AddVar(string id, Expression *e);
	void printall();
	Var GetVar(string id);
	void EmitAcc(ostream &o);
	int index() { return cur_index; };
};

#endif
