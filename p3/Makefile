all:
	lex p3.l
	yacc -v p3.y
	gcc y.tab.c main.c -o prog

clean:
	rm y.tab.c lex.yy.c prog