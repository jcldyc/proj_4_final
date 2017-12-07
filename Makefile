CC = gcc -g
LEX = flex
YACC = bison -y
OBJS = main.o y.tab.o lex.yy.o ST.o \
                tree.o \
                check.o \
                code.o instr.o generate.o

all : turing clean
turing : $(OBJS)
	$(CC) -o turing $(OBJS)
lex.yy.c : scanner.l
	$(LEX) scanner.l
lex.yy.o : lex.yy.c

y.tab.o : y.tab.c y.tab.h tree.h
y.tab.c : parser.y
	$(YACC) parser.y
y.tab.h : parser.y
	$(YACC) -d parser.y
ST.o : ST.c ST.h
tree.o : tree.c tree.h
check.o : check.c tree.h ST.h
main.o : main.c tree.h
clean : 
	rm *.o
cleanall :
	rm lex.yy.c y.tab.c y.tab.h turing

