#ifndef __ACCOUNTING_H__
#define __ACCOUNTING_H__
#include <string>
#include <map>
#include <stack>
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
	int label_offset;
	map<string, Var> vars;
	stack<int> labstack;
public:
	int ar;
	int call_pos;

	Accounting();
	~Accounting();
	bool AddVar(string id, Expression *e);
	Var GetVar(string id);
	void EmitAcc(ostream &o);
	void PushLabel(int label);
	int NewLabel();
	int PopLabel();
	int nLabels();
	int index() { return cur_index; };
	void printall();
};

#endif
