
all: test2
test2: tokensyy.o parser.o
	g++ tokensyy.o parser.o -lflexlexer -o parser

parser.o:
	g++ -c parser.cc

tokensyy.cc: tokens.l
	flex++ -o tokensyy.cc tokens.l

tokensyy.o: tokensyy.cc
	g++ -c tokensyy.cc

clean:
	rm *.o tokensyy.cc
