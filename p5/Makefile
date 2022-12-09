all:
	lex p5.l
	yacc p5.y
	gcc y.tab.c main.c semantico.c code_gen.c -o prog

clean:
	rm y.tab.c lex.yy.c prog