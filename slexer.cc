#include "slexer.h"

SLexer::SLexer()
{
	l = new yyFlexLexer();
}
int SLexer::advance()
{
	if (PEEKED)
		return last_tok;
	return l->yylex();
	
}
void SLexer::over()
{
	if (last_str != NULL)
		delete last_str; // free due to strdup
	PEEKED = 0;
}
char *SLexer::text()
{
	if (PEEKED)
		return last_str;

	return (char *)l->YYText();
}
int SLexer::peek()
{
	PEEKED = 1;
	last_tok = l->yylex();
	last_str = strdup(l->YYText());
	return last_tok;
}

int SLexer::line()
{
	return l->lineno();
}
