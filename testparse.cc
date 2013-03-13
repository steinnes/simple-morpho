#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include "slexer.h"

using namespace std;
int token;

int main(void)
{
	SLexer *lexer = new SLexer();
	do
	{
		token = lexer->advance();
		cout << "Line: " << lexer->line() << " token: " << token << " string: " << lexer->text() << endl;
		// errrr ... F(c) ? .. svo T(c) ? svo hums
	}
	while(token != 0);
}
