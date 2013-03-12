#include "slexer.h"

SLexer::SLexer()
{
	l = new yyFlexLexer();
}
int SLexer::advance()
{
	if (PEEKED)
	{
		PEEKED = 0;
		return last_tok;
	}
	return l->yylex();
	
}
char *SLexer::text()
{
	if (PEEKED) { return last_str; }


	if (last_str != NULL)
		delete last_str; // free due to strdup
	return (char *)l->YYText();
}
int SLexer::peek()
{
	PEEKED = 1;
	last_tok = l->yylex();
	last_str = strdup(l->YYText());
	return last_tok;
}
