#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <FlexLexer.h>

using namespace std;
int token;

int main(void)
{
	FlexLexer *lexer = new yyFlexLexer;
	do
	{
		token = lexer->yylex();
		cout << "Line: " << lexer->lineno() << " token: " << token << " string: " << lexer->YYText() << endl;
		// errrr ... F(c) ? .. svo T(c) ? svo hums
	}
	while(token != 0);
}
