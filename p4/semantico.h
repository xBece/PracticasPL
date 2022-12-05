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
	Desconocido,
} TipoVar;

int get_tipo_lista(TipoVar tipo_basico);
int get_tipo_basico(TipoVar tipo_lista);

bool is_op_rel(TipoOpBinario tipo_op);

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

void ts_check_types(TipoVar tipo1, TipoVar tipo2, const char* msg);

void ts_check_arg_type(const char* func_name, int arg_index, TipoVar tipo);

bool ts_is_var(const char* var_name);

TipoVar ts_get_var_type(const char* var_name);

void ts_check_op_un(TipoVar tipo_var, TipoOpUnario tipo_op);

void ts_check_menos_un(TipoVar tipo);

void ts_check_op_bin(TipoVar tipo_var1, TipoVar tipo_var2, TipoOpBinario tipo_op);

void ts_check_menos_bin(TipoVar tipo1, TipoVar tipo2);

void ts_check_num_args(const char* func_name, int n_given_args);

void ts_check_list_insert(TipoVar tipo_lista, TipoVar tipo_indice, TipoVar tipo_dato);

void ts_check_list_remove(TipoVar tipo_lista, TipoVar tipo_indice);

void ts_check_list_access(TipoVar tipo_lista, TipoVar tipo_indice);

#endif