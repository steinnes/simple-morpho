
all: parser
tester: tokensyy.o testparse.o
	g++ tokensyy.o testparse.o -lfl -o testparse
parser: tokensyy.o parser.o
	g++ tokensyy.o parser.o -lfl -o parser

.cc.o:
	g++ -c $*.cc

tokensyy.cc: tokens.l
	flex++ -o tokensyy.cc tokens.l

tokensyy.o: tokensyy.cc

clean:
	rm tokensyy.cc
	rm parser
	rm *.o
