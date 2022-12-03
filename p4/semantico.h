#ifndef _SEMANTICO_H
#define _SEMANTICO_H

typedef enum {
	Suma,
	Div,
	Mult,
	Greater,
	Less,
	GreaterEq,
	LessEq,
	NotEq,
	Eq,
	And,
	Or,
} TipoOpBinario;

typedef enum {
	Not,
	LengthOf,
} TipoOpUnario;

typedef enum {
	False,
	True,
} TipoBool;

typedef enum {
	Int,
	Bool,
	Float,
	Char,
	ListInt,
	ListBool,
	ListFloat,
	ListChar,
} TipoVar;

int get_tipo_lista(TipoVar tipo_basico);


// Idea: añadir valor para enteros, reales, etc, si fuera necesario en el futuro
// en vez de guardar el lexema.
// TODO: arreglar esta verga. (struct y punto, sin union) UNA POLLA, YA VEREMOS.
typedef struct {
	char* lex;
	int tipo;
} TokenAttrs;

#define YYSTYPE TokenAttrs

void ts_insert_var(const char* lex, TipoVar tipo);

void ts_insert_func(const char* lex);

void ts_insert_param(const char* lex, TipoVar tipo);

void ts_insert_mark(void);

void ts_check(const char* lex);

void ts_remove_until_mark(void);

#endif