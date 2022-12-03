all:
	lex p4.l
	yacc p4.y
	gcc y.tab.c main.c semantico.c -o prog

clean:
	rm y.tab.c lex.yy.c prog