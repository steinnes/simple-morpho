FLAGS = -O0 -ggdb3

all: parser
tester: tokensyy.o testparse.o slexer.o
	g++ tokensyy.o testparse.o slexer.o -lfl -o testparse
parser: tokensyy.o expressions.o accounting.o parser.o slexer.o 
	g++ tokensyy.o parser.o expressions.o accounting.o slexer.o -lfl -o parser

.cc.o:
	g++ $(FLAGS) -c $*.cc

tokensyy.cc: tokens.l
	flex++ -o tokensyy.cc tokens.l

tokensyy.o: tokensyy.cc

clean:
	rm tokensyy.cc
	rm parser
	rm *.o
