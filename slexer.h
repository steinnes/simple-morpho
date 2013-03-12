#ifndef __SLEXER_H__
#define __SLEXER_H__

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
	SLexer();
	int advance();
	char *text();
	int peek();
};

#endif
