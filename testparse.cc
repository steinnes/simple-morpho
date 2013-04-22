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
		Token t = lexer->advance();
		token = t.token;
		cout << "Line: " << t.lineno << " token: " << token << " string: '" << t.lexeme  << "'" << endl;
		// errrr ... F(c) ? .. svo T(c) ? svo hums
	}
	while(token != 0);
}
