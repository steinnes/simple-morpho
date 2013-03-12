%{
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

#define YYSTYPE string
string yylval;

#define NAME   1
#define OP     2
#define VALUE  3
#define VAR    4
#define ERROR -1

int line = 1;

/* Hjálparfall til að afrita les (lexeme) yfir í breytuna yylval,
 * sem er breyta sem bison/yacc kannast við.  Þetta auðveldar seinni
 * tengingu lesgreinisins við bison/yacc þáttara.
 */

static void copy()
{
	char temptext[32768];
	strncpy(temptext,yytext,yyleng);
	temptext[yyleng] = 0;
	yylval = temptext;
}

%}

%%
[ \t]				; //skippa yfir whitespaces
[\n]				line++; // incrementa línur, annars skippa bara yfir
[0-9]+				copy(); return VALUE;
[0-9]*\.[0-9]+			copy(); return VALUE;
"VAR"				copy(); return VAR;
[a-zA-Z][a-zA-Z0-9]*		copy(); return NAME;
[\+\-=<>*/]+			copy(); return OP;
"("|")"|"{"|"}"|","|";"		return (int)yytext[0]; // delimiters
.				return ERROR;
%%

string tok2str(int tok)
{
	char tmpbuf[16];
	memset(tmpbuf, 0, 16);
	switch(tok)
	{
		case NAME:
			return "NAME";
		case VALUE:
			return "VALUE";
		case OP:
			return "OP";
		case ERROR:
			return "ERROR";
		default:
			if (tok > 32 && tok < 127)
			{
				tmpbuf[0] = (char)tok;
				return "DELIMITER: " + string(tmpbuf);
			}
		break;
	}
	return "unknown!";
}

int main( int argc, char **argv ) {
	int token;
	while( (token=yylex()) ) {
		cout << "Line " << line << ", text \"" << yylval << "\", token " << tok2str(token) << endl;
	}
	return 0;
}
