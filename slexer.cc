#include <FlexLexer.h>
#include <string.h>

class SLexer
{
private:
	FlexLexer *l;
	char *last_str;
	int last_tok;
	int PEEKED;
	
public:
	SLexer()
	{
		l = new yyFlexLexer();
	}
	int advance()
	{
		if (PEEKED)
		{
			PEEKED = 0;
			return last_tok;
		}
		return l->yylex();
		
	}
	char *text()
	{
		if (PEEKED) { return last_str; }
		if (last_str != NULL)
			delete last_str; // free due to strdup
		return l->YYText();
	}
	int peek()
	{
		PEEKED = 1;
		last_tok = l->yylex();
		last_str = strdup(l->YYText());
		return last_tok;
	}
};

